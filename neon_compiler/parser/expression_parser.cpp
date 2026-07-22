#include "expression_parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::ast::nodes;
using namespace neon_compiler::analysis;

const Token& ExpressionParser::peek_w_peek_cursor(PeekCursor peek_cursor, uint offset)
{
	return reader->peek((peek_cursor ? *peek_cursor : 0) + offset);
}

const Token& ExpressionParser::consume_w_peek_cursor(PeekCursor peek_cursor, uint offset)
{
	if(peek_cursor)
	{
		const Token& token = peek_w_peek_cursor(peek_cursor, offset);
		*peek_cursor += offset + 1;
		return token;
	}
	else
	{
		const Token& token = reader->consume(offset);
		return token;
	}
}

const neon_compiler::Token& ExpressionParser::consume_w_peek_cursor_and_report
(
	AnalysisEntryType type,
	AnalysisSeverity severity,
	PeekCursor peek_cursor,
	std::optional<std::string> info
)
{
	if(peek_cursor)
	{
		return consume_w_peek_cursor(peek_cursor);
	}
	else
	{
		const Token& token = consume_w_peek_cursor(peek_cursor);
		(*func_report_token)(type, severity, token, info);
		return token;
	}
}

std::optional<neon_compiler::ast::Identifier> ExpressionParser::parse_identifier(AnalysisEntryType type, AnalysisSeverity severity, PeekCursor peek_cursor)
{
	std::vector<std::string> parts;
	std::vector<Token> tokens;

	bool continue_reading{true};
	do
	{
		if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::IDENTIFIER)
		{
			return std::nullopt;
		}

		Token token = consume_w_peek_cursor(peek_cursor);
		tokens.push_back(token);
		parts.emplace_back(token.get_lexeme().value());

		continue_reading = peek_w_peek_cursor(peek_cursor).get_type() == TokenType::STATIC_ACCESSOR;
		if(continue_reading)
		{
			tokens.push_back(consume_w_peek_cursor(peek_cursor));
		}
	}
	while(continue_reading);

	neon_compiler::ast::Identifier id{parts};
	std::string id_string = id.to_string();

	if(!peek_cursor)
	{
		for(const Token& token : tokens)
		{
			(*func_report_token)(type, severity, token, id_string);
		}
	}

	return id;
}

std::unique_ptr<Expression> ExpressionParser::parse_expression(PeekCursor peek_cursor, uint max_subordination)
{
	// Implements Pratt parsing, but with subordination instead of precedence

	FuncParseExpressionWCursor func_parse_expression_w_cursor = [this] (uint peek_offset, uint max_subordination)
	{
		parse_expression(&peek_offset, max_subordination);
		return peek_offset;
	};

	std::unique_ptr<Expression> left = parse_prefix_expression(peek_cursor, func_parse_expression_w_cursor);

	if(!left)
	{
		logger->info("Parsing encountered an invalid expression.");
		return nullptr;
	}

	while(true)
	{
		std::shared_ptr<const Operator> op = operator_table->match_infix(*reader, peek_cursor, func_parse_expression_w_cursor);

		if(!op) { op = operator_table->match_postfix(*reader, peek_cursor, func_parse_expression_w_cursor); }

		if(!op) { break; }

		if(op->get_declaration()->subordination > max_subordination) { break; }

		left = parse_operator_call_expression(peek_cursor, op, std::move(left));
	}

	return left;
}

std::unique_ptr<Expression> ExpressionParser::parse_prefix_expression(PeekCursor peek_cursor, FuncParseExpressionWCursor func_parse_expression_w_cursor)
{
	{
		std::unique_ptr<Expression> expr = parse_parenthesised_expression(peek_cursor);
		if(expr) { return expr; }
	}

	{
		// Handle prefix operators
		std::shared_ptr<const Operator> op = operator_table->match_prefix(*reader, peek_cursor, func_parse_expression_w_cursor);
		if(op) { return parse_operator_call_expression(peek_cursor, op); }
	}

	{
		std::unique_ptr<Expression> expr = parse_terminating_expression(peek_cursor);
		if(expr) { return expr; }
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, peek_cursor,
		std::string{expression_error_messages::INVALID_EXPRESSION});

	return nullptr;
}


std::unique_ptr<Expression> ExpressionParser::parse_terminating_expression(PeekCursor peek_cursor)
{
	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::LITERAL_NUMBER)
	{
		return std::make_unique<LiteralNumberExpression>
		(
			std::string{consume_w_peek_cursor_and_report(AnalysisEntryType::LITERAL_NUMBER, AnalysisSeverity::INFO, peek_cursor)
				.get_lexeme().value()}
		);
	}

	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::LITERAL_STRING)
	{
		return std::make_unique<LiteralStringExpression>
		(
			std::string{consume_w_peek_cursor_and_report(AnalysisEntryType::LITERAL_STRING, AnalysisSeverity::INFO, peek_cursor)
				.get_lexeme().value()}
		);
	}

	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::BOOL_TRUE)
	{
		consume_w_peek_cursor_and_report(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, peek_cursor);
		return std::make_unique<LiteralBooleanExpression>(true);
	}

	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::BOOL_FALSE)
	{
		consume_w_peek_cursor_and_report(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, peek_cursor);
		return std::make_unique<LiteralBooleanExpression>(false);
	}

	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::IDENTIFIER)
	{
		return parse_named_expression(peek_cursor);
	}

	return nullptr;
}

std::unique_ptr<Expression> ExpressionParser::parse_parenthesised_expression(PeekCursor peek_cursor)
{
	if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::BRACKET_ROUND_OPEN)
	{
		return nullptr;
	}
	
	consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor);

	std::unique_ptr<Expression> expr = parse_expression(peek_cursor);
	if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::BRACKET_ROUND_CLOSE)
	{
		consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor);
	}
	else
	{
		consume_w_peek_cursor_and_report(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, peek_cursor,
			std::string{expression_error_messages::INVALID_PARENTHESISED_EXPRESSION__EXPECTED_CLOSING_BRACKET});
	}

	return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_named_expression(PeekCursor peek_cursor)
{
	// At this point, an identifier should be guaranteed
	neon_compiler::ast::Identifier id{parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO, peek_cursor).value()};

	if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::BRACKET_ROUND_OPEN)
	{
		return std::make_unique<SimpleRead>(id.to_string());
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor);

	std::vector<std::unique_ptr<Expression>> argument_expressions;
	
	if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::BRACKET_ROUND_CLOSE)
	{
		argument_expressions = parse_argument_expressions(peek_cursor);
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor); // Consume the `)`

	return std::make_unique<FunctionCall>(id.to_string(), std::move(argument_expressions));
}

std::vector<std::unique_ptr<Expression>> ExpressionParser::parse_argument_expressions(PeekCursor peek_cursor)
{
	std::vector<std::unique_ptr<Expression>> argument_expressions;

	while(!reader->end_of_file_reached(peek_cursor ? *peek_cursor : 0))
	{
		argument_expressions.push_back(std::move(parse_expression(peek_cursor)));

		if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::BRACKET_ROUND_CLOSE)
		{
			return argument_expressions;
		}

		if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::COMMA)
		{
			consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::ERROR, peek_cursor,
				std::string{expression_error_messages::INVALID_ARGUMENT_LIST__EXPECTED_COMMA_OR_CLOSING_BRACKET});
		}
		else
		{
			consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor);
		}
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::ERROR, peek_cursor,
		std::string{expression_error_messages::UNEXPECTED_END_OF_FILE_IN_ARGUMENT_LIST});

	return argument_expressions;
}

std::unique_ptr<Expression> ExpressionParser::parse_operator_call_expression
(
	PeekCursor peek_cursor,
	std::shared_ptr<const Operator> op,
	std::unique_ptr<Expression> first_argument
)
{
	if(op->get_declaration()->builtin_operator_kind == BuiltinOperatorKind::MEMBER_ACCESS)
	{
		return parse_dot_expression(peek_cursor, std::move(first_argument));
	}

	std::vector<std::unique_ptr<Expression>> arguments;

	bool first_argument_passed{first_argument};

	if(first_argument_passed) { arguments.push_back(std::move(first_argument)); }

	std::shared_ptr<const neon_compiler::ast::nodes::OperatorDeclaration> declaration = op->get_declaration();
	const std::vector<OperatorSyntaxPatternElement>& pattern = declaration->pattern;

	for(std::size_t i = (first_argument_passed ? 1 : 0); i < pattern.size(); ++i)
	{
		const OperatorSyntaxPatternElement& elem = pattern[i];

		if(std::holds_alternative<TokenPattern>(elem))
		{
			consume_w_peek_cursor_and_report(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, peek_cursor);
			continue;
		}

		uint max_subordination = declaration->subordination - 1;
		if(i == pattern.size() - 1 && declaration->associativity == OperatorAssociativity::RIGHT) { ++max_subordination; }
		std::unique_ptr<Expression> argument = parse_expression(peek_cursor, max_subordination);

		if(!argument)
		{
			logger->info("Operator call expression failed to parse argument. (pattern element index: " + std::to_string(i) + ")");
			return nullptr;
		}

		arguments.push_back(std::move(argument));
	}

	return std::make_unique<OperatorCallExpression>(std::move(arguments), op);
}

std::unique_ptr<Expression> ExpressionParser::parse_dot_expression
(
	PeekCursor peek_cursor,
	std::unique_ptr<Expression> first_argument
)
{
	// Consume the dot
	consume_w_peek_cursor_and_report(AnalysisEntryType::OPERATOR, AnalysisSeverity::INFO, peek_cursor);

	// The dot operator must be declared so first_argument always has a value here.

	if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::IDENTIFIER)
	{
		consume_w_peek_cursor_and_report(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, peek_cursor);
		return nullptr;
	}

	std::unique_ptr<Expression> second_argument = parse_named_expression(peek_cursor);

	if(dynamic_cast<SimpleRead*>(second_argument.get()))
	{
		std::unique_ptr<SimpleRead> read =
			std::unique_ptr<SimpleRead>
			(
				static_cast<SimpleRead*>(second_argument.release())
			);
		return std::make_unique<ObjectRead>(std::move(first_argument), read->reference_name);
	}
	else
	{
		std::unique_ptr<FunctionCall> call =
			std::unique_ptr<FunctionCall>
			(
				static_cast<FunctionCall*>(second_argument.release())
			);
		return std::make_unique<ObjectFunctionCall>(std::move(first_argument), call->function_name, std::move(call->arguments));
	}
}
