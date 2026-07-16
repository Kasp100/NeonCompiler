#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;
using namespace neon_compiler::ast::nodes;

Parser::Parser
(
	std::shared_ptr<logging::Logger> logger,
	std::span<const Token> tokens,
	std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter,
	std::shared_ptr<neon_compiler::ast::nodes::Root> root_node,
	std::string_view file
) :
	logger{logger}, reader{tokens}, analysis_reporter{analysis_reporter}, root_node{root_node}, file{file} {}

void Parser::run()
{
	parse_and_register_expected_package_declaration();

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::IMPORT)
		{
			parse_and_register_import_statement();
		}
		else
		{
			const Access access = parse_access(); // `private` if no keyword is present.
			parse_expected_package_member(access);
		}
	}
}

std::shared_ptr<neon_compiler::ast::nodes::Root> Parser::get_root_node() const
{
	return root_node;
}

const Token& Parser::peek_w_peek_cursor(PeekCursor peek_cursor, uint offset)
{
	return reader.peek((peek_cursor ? *peek_cursor : 0) + offset);
}

const Token& Parser::consume_w_peek_cursor(PeekCursor peek_cursor, uint offset)
{
	if(peek_cursor)
	{
		const Token& token = peek_w_peek_cursor(peek_cursor, offset);
		*peek_cursor += offset + 1;
		return token;
	}
	else
	{
		const Token& token = reader.consume(offset);
		return token;
	}
}

const neon_compiler::Token& Parser::consume_w_peek_cursor_and_report
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
		return report_token(type, severity, consume_w_peek_cursor(peek_cursor), info);
	}
}

const neon_compiler::Token& Parser::report_token
(
	AnalysisEntryType type,
	AnalysisSeverity severity,
	const neon_compiler::Token& token,
	std::optional<std::string> info
)
{
	analysis_reporter->report(AnalysisEntry{file, type, severity, token.get_source_position(), token.get_length(), info});
	return token;
}

void Parser::append_ast(std::unique_ptr<PackageMember> node, const std::string& identifier)
{
	const std::string full_identifier{package.to_string() + "::" + identifier};

	root_node->file_package_members[std::string{file}].push_back(full_identifier);
	root_node->package_members[full_identifier] = std::move(node);
}

std::optional<neon_compiler::ast::Identifier> Parser::parse_identifier(AnalysisEntryType type, AnalysisSeverity severity, PeekCursor peek_cursor)
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
			report_token(type, severity, token, id_string);
		}
	}

	return id;
}

void Parser::parse_and_register_expected_package_declaration()
{
	if(reader.peek().get_type() == TokenType::PACKAGE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	std::optional<neon_compiler::ast::Identifier> package_id = parse_identifier(AnalysisEntryType::PACKAGE, AnalysisSeverity::INFO);
	if(!package_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	if(reader.peek().get_type() == TokenType::END_STATEMENT)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_SEMICOLON});
	}

	package = package_id.value_or(neon_compiler::ast::Identifier{});
}

void Parser::parse_and_register_import_statement()
{
	// At this point, IMPORT should be guaranteed
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::optional<neon_compiler::ast::Identifier> package_member_id = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(!package_member_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::INVALID_IMPORT_STATEMENT});
		return;
	}

	imports.push_back(package_member_id.value());
}

Access Parser::parse_access()
{
	Access access{AccessType::PRIVATE};
	if(reader.peek().get_type() == TokenType::ACCESS_PRIVATE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PROTECTED)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::PROTECTED_PACKAGE_MEMBER});
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PUBLIC)
	{
		access = Access{AccessType::PUBLIC};
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_EXCLUSIVE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::MISSING_PACKAGE_MEMBER_PATTERNS});
		}

		std::vector<PackageMemberPattern> patterns;

		patterns.push_back(parse_package_member_pattern());
		while(reader.peek().get_type() == TokenType::COMMA)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			patterns.push_back(parse_package_member_pattern());
		}

		access = Access{AccessType::EXCLUSIVE, patterns};

		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_CLOSING_BRACKET});
		}
	}
	return access;
}

PackageMemberPattern Parser::parse_package_member_pattern()
{
	PackageMemberPatternType type = PackageMemberPatternType::PACKAGE_MEMBER;
	if(reader.peek().get_type() == TokenType::SHALLOW)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		type = PackageMemberPatternType::PACKAGE_WITHOUT_SUBPACKAGES;
	}
	else if(reader.peek().get_type() == TokenType::DEEP)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		type = PackageMemberPatternType::PACKAGE_WITH_SUBPACKAGES;
	}

	if(type != PackageMemberPatternType::PACKAGE_MEMBER)
	{
		if(reader.peek().get_type() == TokenType::PACKAGE)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_PKG});
		}
	}

	std::optional<ast::Identifier> package_member_identifier = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(reader.peek().get_type() != TokenType::INHERITANCE_EXTENDS) { return PackageMemberPattern{type, package_member_identifier}; }
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::optional<ast::Identifier> supertype = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);
	if(supertype.has_value())
	{
		return PackageMemberPattern{package_member_identifier.has_value() ? PackageMemberPatternType::INHERITANCE_ONLY : type,
			package_member_identifier, supertype};
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SECOND_PACKAGE_MEMBER_PATTERN});

		return PackageMemberPattern{type, package_member_identifier};
	}
}

void Parser::parse_expected_package_member(const Access& access)
{
	if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_ENTRYPOINT)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_expected_entrypoint(access);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_PURE_FUNCTION_SET)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_OPERATOR_MODULE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_expected_operator_module(access);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_COMPILE_FUNCTION)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::MEMBER_ABSTRACT && reader.peek(1).get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_INTERFACE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_FILE_LEVEL_TOKEN});
	}
}

std::string Parser::parse_expected_declaration_name(AnalysisEntryType analysis_entry_type)
{
	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		std::string name{reader.peek().get_lexeme().value()};
		report_token(analysis_entry_type, AnalysisSeverity::INFO, reader.consume(), name);
		return name;
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_DECLARATION_NAME});

		return std::string{error_recovery::PLACEHOLDER_NAME};
	}
}

void Parser::parse_and_register_expected_entrypoint(const Access& access)
{
	const std::string name = parse_expected_declaration_name(AnalysisEntryType::DECLARATION);

	ParameterDeclarationList parameters = parse_parameter_declarations();

	CodeBlock body = CodeBlock{std::vector<std::unique_ptr<Statement>>{}};

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		body = parse_code_block_until_end();
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_CODE_BLOCK});
	}

	std::unique_ptr<PackageMember> package_member = std::make_unique<Entrypoint>(access, std::move(parameters), std::move(body));

	append_ast(std::move(package_member), name);
}

// TODO: write tests for this
void Parser::parse_and_register_expected_operator_module(const Access& access)
{
	const std::string name = parse_expected_declaration_name(AnalysisEntryType::DECLARATION);

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_CODE_BLOCK});
	}

	std::vector<OperatorFunction> functions;

	while(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		std::optional<ReferenceType> return_value = parse_reference_type(MutabilityMode::BORROW);
		if(!return_value.has_value())
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_REFERENCE_TYPE});
		}

		std::vector<OperatorFunctionPatternElement> pattern = parse_operator_function_pattern();
		if(reader.end_of_file_reached()) { return; }

		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume the `{`
		CodeBlock body = parse_code_block_until_end();

		functions.emplace_back
		(
			return_value.value_or(ReferenceType{false, MutabilityMode::BORROW, false, std::string{error_recovery::PLACEHOLDER_TYPE}}),
			std::move(pattern),
			std::move(body)
		);
	}

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_OPERATOR_FUNCTION_OR_MISSING_CLOSING_BRACKET});
		// TODO: go to synchronisation token (create a helper method for this)
	}

	std::unique_ptr<PackageMember> package_member = std::make_unique<OperatorModule>(access, std::move(functions));

	append_ast(std::move(package_member), name);
}

std::vector<OperatorFunctionPatternElement> Parser::parse_operator_function_pattern()
{
	std::vector<OperatorFunctionPatternElement> pattern;

	while(!reader.end_of_file_reached() && reader.peek().get_type() != TokenType::BRACKET_CURLY_OPEN)
	{
		if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

			VariableDeclaration parameter = parse_variable_declaration(MutabilityMode::BORROW)
				.value_or
				(
					VariableDeclaration
					{
						false,
						ReferenceType{false, MutabilityMode::BORROW, false, std::string{error_recovery::PLACEHOLDER_TYPE}},
						std::string{error_recovery::PLACEHOLDER_NAME}
					}
				);

			pattern.push_back(OperatorFunctionParameter{parameter});

			if(reader.peek().get_type() == TokenType::BRACKET_ROUND_CLOSE)
			{
				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			}
			else
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
					std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN_PART__EXPECTED_CLOSING_BRACKET});
			}
		}
		else
		{
			const Token& token = reader.peek();
			std::optional<std::string> lexeme{std::nullopt};

			if(token.get_lexeme().has_value())
			{
				lexeme = std::string{token.get_lexeme().value()};
			}
	
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	
			pattern.push_back(TokenPattern{token.get_type(), lexeme});
		}
	}

	return pattern;
}

ParameterDeclarationList Parser::parse_parameter_declarations()
{
	ParameterDeclarationList param_decl_list{};

	if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		return param_decl_list;
	}

	bool first{true};

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::BRACKET_ROUND_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}
		else if(!first)
		{
			if(reader.peek().get_type() == TokenType::COMMA)
			{
				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			}
			else
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
					std::string{error_messages::INVALID_PARAMETER_DECLARATION});
				continue;
			}
		}

		first = false;

		std::optional<VariableDeclaration> var_decl = parse_variable_declaration(MutabilityMode::BORROW);

		if(!var_decl.has_value())
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PARAMETER_DECLARATION});
			continue;
		}

		param_decl_list.parameters.push_back(std::move(var_decl.value()));
	}

	return param_decl_list;
}

std::optional<VariableDeclaration> Parser::parse_variable_declaration(MutabilityMode default_mutability_mode)
{
	bool var{reader.peek().get_type() == TokenType::VAR};

	if(var)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	std::optional<ReferenceType> ref_type = parse_reference_type(default_mutability_mode);

	std::string ref_name{error_recovery::PLACEHOLDER_NAME};
	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		report_token(AnalysisEntryType::DECLARATION, AnalysisSeverity::INFO, reader.peek());
		ref_name = reader.consume().get_lexeme().value();
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_IDENTIFIER});
	}

	if(ref_type.has_value())
	{
		return VariableDeclaration{var, ref_type.value(), ref_name}; // TODO: parse optional expression for initialisation
	}
	else if(var)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_VARIABLE_DECLARATION});

		ReferenceType valid_ref_type{false, default_mutability_mode, false, std::string{error_recovery::PLACEHOLDER_NAME}};

		return VariableDeclaration{var, valid_ref_type, ref_name};
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<ReferenceType> Parser::parse_reference_type(MutabilityMode default_mutability_mode)
{
	bool opt{reader.peek().get_type() == TokenType::OPTIONAL};

	if(opt)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	TokenType tt{reader.peek().get_type()};

	MutabilityMode mm{default_mutability_mode};
	bool implicit_mutability_mode{false};

	switch (tt)
	{
		case TokenType::REF_TYPE_OWN:     mm = MutabilityMode::OWN;         break;
		case TokenType::REF_TYPE_SHARED:  mm = MutabilityMode::SHARED;      break;
		case TokenType::REF_TYPE_BORROW:  mm = MutabilityMode::BORROW;      break;
		default:                          implicit_mutability_mode = true;  break;
	}

	if(!implicit_mutability_mode)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	bool mut{reader.peek().get_type() == TokenType::MUT};

	if(mut)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		if(reader.peek().get_type() == TokenType::COLON) { report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); }
	}

	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		report_token(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO, reader.peek());
		std::string type_name{reader.consume().get_lexeme().value()};
		return ReferenceType{opt, mm, mut, type_name};
	}
	else if(opt || mut || !implicit_mutability_mode)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_REFERENCE_TYPE});

		return ReferenceType{opt, mm, mut, std::string{error_recovery::PLACEHOLDER_NAME}};
	}
	else
	{
		return std::nullopt;
	}
}

CodeBlock Parser::parse_code_block_until_end()
{
	// At this point, a `{` should already be consumed
	std::vector<std::unique_ptr<Statement>> statements;

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}
		else if(reader.peek().get_type() == TokenType::STMT_RETURN)
		{
			statements.push_back(std::move(parse_return_statement()));
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume());
		}
	}

	return CodeBlock{std::move(statements)};
}

std::unique_ptr<Statement> Parser::parse_return_statement()
{
	// At this point, STMT_RETURN should be guaranteed
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::unique_ptr<Expression> value = nullptr;
	
	if(reader.peek().get_type() != TokenType::END_STATEMENT)
	{
		value = parse_expression();
	}

	if(reader.peek().get_type() == TokenType::END_STATEMENT)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SEMICOLON});
	}

	return std::make_unique<Return>(std::move(value));
}

std::unique_ptr<Expression> Parser::parse_expression(PeekCursor peek_cursor, uint max_subordination)
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
		std::shared_ptr<const Operator> op = operator_table.match_infix(reader, peek_cursor, func_parse_expression_w_cursor);

		if(!op) { op = operator_table.match_postfix(reader, peek_cursor, func_parse_expression_w_cursor); }

		if(!op) { break; }

		if(op->get_declaration()->subordination > max_subordination) { break; }

		left = parse_operator_call_expression(peek_cursor, op, std::move(left));
	}

	return left;
}

std::unique_ptr<Expression> Parser::parse_prefix_expression(PeekCursor peek_cursor, FuncParseExpressionWCursor func_parse_expression_w_cursor)
{
	{
		std::unique_ptr<Expression> expr = parse_parenthesised_expression(peek_cursor);
		if(expr) { return expr; }
	}

	{
		// Handle prefix operators
		std::shared_ptr<const Operator> op = operator_table.match_prefix(reader, peek_cursor, func_parse_expression_w_cursor);
		if(op) { return parse_operator_call_expression(peek_cursor, op); }
	}

	{
		std::unique_ptr<Expression> expr = parse_terminating_expression(peek_cursor);
		if(expr) { return expr; }
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, peek_cursor,
		std::string{error_messages::INVALID_EXPRESSION});

	return nullptr;
}


std::unique_ptr<Expression> Parser::parse_terminating_expression(PeekCursor peek_cursor)
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

std::unique_ptr<Expression> Parser::parse_parenthesised_expression(PeekCursor peek_cursor)
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
			std::string{error_messages::INVALID_CALL_EXPRESSION__EXPECTED_CLOSING_BRACKET});
	}

	return expr;
}

std::unique_ptr<Expression> Parser::parse_named_expression(PeekCursor peek_cursor)
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

std::vector<std::unique_ptr<Expression>> Parser::parse_argument_expressions(PeekCursor peek_cursor)
{
	std::vector<std::unique_ptr<Expression>> argument_expressions;

	while(!reader.end_of_file_reached(peek_cursor ? *peek_cursor : 0))
	{
		argument_expressions.push_back(std::move(parse_expression(peek_cursor)));

		if(peek_w_peek_cursor(peek_cursor).get_type() == TokenType::BRACKET_ROUND_CLOSE)
		{
			return argument_expressions;
		}

		if(peek_w_peek_cursor(peek_cursor).get_type() != TokenType::COMMA)
		{
			consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::ERROR, peek_cursor,
				std::string{error_messages::INVALID_ARGUMENT_LIST__EXPECTED_COMMA_OR_CLOSING_BRACKET});
		}
		else
		{
			consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, peek_cursor);
		}
	}

	consume_w_peek_cursor_and_report(AnalysisEntryType::SEPARATOR, AnalysisSeverity::ERROR, peek_cursor,
		std::string{error_messages::UNEXPECTED_END_OF_FILE_IN_ARGUMENT_LIST});

	return argument_expressions;
}

std::unique_ptr<Expression> Parser::parse_operator_call_expression
(
	PeekCursor peek_cursor,
	std::shared_ptr<const Operator> op,
	std::unique_ptr<Expression> first_argument
)
{
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
		arguments.push_back(parse_expression(peek_cursor, max_subordination));
	}

	return std::make_unique<OperatorCallExpression>(std::move(arguments), op);
}
