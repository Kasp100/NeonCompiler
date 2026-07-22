#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include "operator_table.hpp"
#include "../ast/nodes/statement_nodes.hpp"
#include "../analysis/analysis_entry.hpp"
#include "../../logging/logger.hpp"

namespace neon_compiler::parser
{

namespace expression_error_messages
{
	constexpr std::string_view INVALID_EXPRESSION =
		"Invalid expression; see documentation about expressions.";
	constexpr std::string_view INVALID_PARENTHESISED_EXPRESSION__EXPECTED_CLOSING_BRACKET =
		"Invalid parenthesised expression; expected closing bracket `)`.";
	constexpr std::string_view INVALID_ARGUMENT_LIST__EXPECTED_COMMA_OR_CLOSING_BRACKET =
		"Invalid argument list; expected comma or closing bracket after argument.";
	constexpr std::string_view UNEXPECTED_END_OF_FILE_IN_ARGUMENT_LIST =
		"Unexpected end of file in argument list; a closing bracket `)` is missing.";
}

using FuncReportToken = std::function
<
	void
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		const neon_compiler::Token& token,
		std::optional<std::string> info
	)
>;

class ExpressionParser
{
public:
	explicit ExpressionParser
	(
		std::shared_ptr<logging::Logger> logger,
		neon_compiler::TokenReader* reader,
		FuncReportToken* func_report_token,
		neon_compiler::parser::OperatorTable* operator_table
	)
	: logger{logger}, reader{reader}, func_report_token{func_report_token}, operator_table{operator_table} {}

	std::optional<neon_compiler::ast::Identifier> parse_identifier
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		PeekCursor peek_cursor = nullptr
	);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_expression(PeekCursor peek_cursor = nullptr, uint max_subordination = INT_MAX);
private:
	std::shared_ptr<logging::Logger> logger;
	neon_compiler::TokenReader* reader;
	FuncReportToken* func_report_token;
	neon_compiler::parser::OperatorTable* operator_table;

	const neon_compiler::Token& peek_w_peek_cursor(PeekCursor peek_cursor, uint offset = 0);
	const neon_compiler::Token& consume_w_peek_cursor(PeekCursor peek_cursor, uint offset = 0);
	const neon_compiler::Token& consume_w_peek_cursor_and_report
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		PeekCursor peek_cursor = nullptr,
		std::optional<std::string> info = std::nullopt
	);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_prefix_expression(PeekCursor peek_cursor, FuncParseExpressionWCursor func_parse_expression_w_cursor);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_terminating_expression(PeekCursor peek_cursor);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_parenthesised_expression(PeekCursor peek_cursor);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_named_expression(PeekCursor peek_cursor);
	std::vector<std::unique_ptr<neon_compiler::ast::nodes::Expression>> parse_argument_expressions(PeekCursor peek_cursor);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_operator_call_expression
	(
		PeekCursor peek_cursor,
		std::shared_ptr<const neon_compiler::parser::Operator> op,
		std::unique_ptr<neon_compiler::ast::nodes::Expression> first_argument = nullptr
	);
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_dot_expression
	(
		PeekCursor peek_cursor,
		std::unique_ptr<neon_compiler::ast::nodes::Expression> first_argument
	);
};

}

#endif // EXPRESSION_PARSER_HPP