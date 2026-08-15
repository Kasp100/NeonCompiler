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
		"Invalid expression. The required operator module may not be active in this scope.";

	constexpr std::string_view INVALID_PARENTHESISED_EXPRESSION__EXPECTED_CLOSING_BRACKET =
		"Invalid parenthesised expression. Expected `)` to close the expression. The required operator module may not be active in this scope.";

	constexpr std::string_view INVALID_ARGUMENT_EXPRESSION =
		"Invalid argument expression. Expected `,` or `)` after the argument. The required operator module may not be active in this scope.";

	constexpr std::string_view UNEXPECTED_END_OF_FILE_IN_ARGUMENT_LIST =
		"Unexpected end of file in argument list. Expected `)`.";

	constexpr std::string_view INVALID_GENERIC_ARGUMENT =
		"Invalid generic argument. Expected a number or boolean literal, or a type or constant reference.";
}

using FuncReportToken = std::function
<
	void
	(
		neon_compiler::analysis::AnalysisEntryType report_type,
		neon_compiler::analysis::AnalysisSeverity report_severity,
		const neon_compiler::Token& token,
		std::optional<std::string> info
	)
>;

class ExpressionParser
{
public:
	explicit ExpressionParser
	(
		std::shared_ptr<logging::Logger> init_logger,
		neon_compiler::TokenReader* init_reader,
		FuncReportToken* init_func_report_token,
		neon_compiler::parser::OperatorTable* init_operator_table
	);

	std::optional<neon_compiler::ast::Identifier> parse_identifier
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		PeekCursor peek_cursor = nullptr
	);

	std::vector<neon_compiler::ast::nodes::GenericArgument> parse_generic_arguments(PeekCursor peek_cursor = nullptr);

	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_expression
	(
		PeekCursor peek_cursor = nullptr,
		uint max_subordination = INT_MAX
	);
private:
	static constexpr std::string_view VALUE_FALSE = "false";
	static constexpr std::string_view VALUE_TRUE = "true";

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
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_member_access_dot_expression
	(
		PeekCursor peek_cursor,
		std::unique_ptr<neon_compiler::ast::nodes::Expression> first_argument
	);
};

}

#endif // EXPRESSION_PARSER_HPP