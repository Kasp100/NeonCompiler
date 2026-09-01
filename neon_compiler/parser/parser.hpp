#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include <optional>
#include <string>
#include "expression_parser.hpp"
#include "operator_table.hpp"
#include "../ast/nodes/nodes.hpp"
#include "../ast/nodes/statement_nodes.hpp"
#include "../token.hpp"
#include "../token_reader.hpp"
#include "../analysis/analysis_entry.hpp"
#include "../analysis/analysis_reporter.hpp"
#include "../ast/package_member_id.hpp"
#include "../../logging/logger.hpp"

namespace neon_compiler::parser
{

namespace error_messages
{
	constexpr std::string_view UNEXPECTED_END_OF_FILE =
		"Unexpected end of file. A terminating token is missing. Statements end with `;`, and blocks end with `}`.";

	constexpr std::string_view MISSING_PACKAGE_DECLARATION =
		"Expected a package declaration (e.g., `pkg main::example;`).";

	constexpr std::string_view MISSING_IDENTIFIER =
		"Expected an identifier. Keywords and other tokens are not valid identifiers.";

	constexpr std::string_view PROTECTED_PACKAGE_MEMBER =
		"`protected` is only valid for type members, not package members.";

	constexpr std::string_view INVALID_FILE_LEVEL_TOKEN =
		"Invalid token at file scope. See the language documentation.";

	constexpr std::string_view INVALID_IMPORT_STATEMENT =
		"Expected a package member reference (e.g., `my_package::my_class`) in the import statement.";

	constexpr std::string_view INVALID_DECLARATION_NAME =
		"Invalid declaration name. Keywords and other tokens are not valid names.";

	constexpr std::string_view INVALID_REFERENCE_TYPE =
		"Expected a reference type (e.g., `opt int`).";

	constexpr std::string_view INVALID_VARIABLE_DECLARATION =
		"Expected a variable declaration (e.g., `var int age`).";

	constexpr std::string_view INVALID_PARAMETER_DECLARATION =
		"Expected a parameter declaration (e.g., `shared mut:array<int> arr`). Separate parameters with `,` and end the parameter list with `)`.";

	constexpr std::string_view MISSING_SEMICOLON =
		"Expected `;`.";

	constexpr std::string_view MISSING_SEMICOLON_OR_FAILED_TO_PARSE_EXPRESSION =
		"Expected `;` or a valid expression. The required operator module may not be active in this scope.";

	constexpr std::string_view MISSING_SECOND_PACKAGE_MEMBER_PATTERN =
		"Expected a second package member pattern.";

	constexpr std::string_view MISSING_PACKAGE_MEMBER_PATTERNS =
		"Expected one or more package member patterns enclosed in `{}`.";

	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_CLOSING_BRACKET =
		"Invalid package member pattern. Expected `}`.";

	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_PKG =
		"Invalid package member pattern. Expected `pkg` after `shallow` or `deep`.";

	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN_PART__EXPECTED_CLOSING_BRACKET =
		"Expected `)` to end the parameter declaration.";

	constexpr std::string_view INVALID_SUBORDINATION =
		"Invalid subordination. Expected a non-negative integer, where `0` represents the highest precedence.";

	constexpr std::string_view INVALID_ASSOCIATIVITY =
		"Invalid associativity. Expected `left`, `right`, or no associativity.";

	constexpr std::string_view INVALID_OPERATOR_PROPERTY =
		"Invalid operator property. Supported properties are `subordination` (precedence) and `associativity`.";

	constexpr std::string_view INVALID_USE_STATEMENT_ARGUMENT =
		"Invalid `use` statement. Expected a reference to an operator module.";

	constexpr std::string_view INVALID_GENERIC_PARAMETER_TYPE =
		"Invalid generic parameter. Expected a generic parameter type. See documentation about generics.";

	constexpr std::string_view INVALID_GENERIC_PARAMETER_SEPARATOR =
		"Invalid generic parameter. Expected `>` or `,`.";

	constexpr std::string_view INVALID_SUPERTYPE_LIST_IDENTIFIER =
		"Invalid supertype list. Expected a valid type identifier.";

	constexpr std::string_view INVALID_OPERATOR_MODULE__MISSING_CURLY_BRACKETS =
		"Invalid operator module. Expected `{}`.";

	constexpr std::string_view INVALID_FUNCTION_EFFECT =
		"Invalid function effect. Only `io` is allowed for regular functions.";

	constexpr std::string_view INVALID_FUNCTION__MISSING_CODE_BLOCK =
		"Invalid function. Expected a code block enclosed in `{}` and no keywords.";
}

namespace error_recovery
{
	constexpr std::string_view PLACEHOLDER_NAME = "err_name";
	constexpr std::string_view PLACEHOLDER_TYPE_NAME = "err_type";
	const neon_compiler::ast::PackageMemberID PLACEHOLDER_TYPE_ID{std::vector<std::string>{std::string{PLACEHOLDER_TYPE_NAME}}};
}

using OperatorMap = std::unordered_map<std::string, std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>>;

class Parser
{
public:
	explicit Parser
	(
		std::shared_ptr<logging::Logger> init_logger,
		std::span<const neon_compiler::Token> init_tokens,
		std::shared_ptr<neon_compiler::analysis::AnalysisReporter> init_analysis_reporter,
		std::shared_ptr<neon_compiler::ast::nodes::Root> init_root_node,
		std::string_view init_file,
		std::shared_ptr<OperatorMap> init_operator_map
	);

	/** Should be run first in the parsing phase to register package declaration and parse operator modules. */
	void run_a();

	/** Should be run second in the parsing phase to parse other package member types. */
	void run_b(std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table);

	std::shared_ptr<neon_compiler::ast::nodes::Root> get_root_node() const;
private:
	std::shared_ptr<logging::Logger> logger;
	neon_compiler::TokenReader reader;
	std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter;
	std::shared_ptr<neon_compiler::ast::nodes::Root> root_node;
	std::string_view file;
	neon_compiler::ast::PackageMemberID package;

	/** Mapping from reference name to declaration path */
	std::unordered_map<std::string, neon_compiler::ast::PackageMemberID> imports;
	/** Mapping from package member identifier to operator lists */
	std::shared_ptr<OperatorMap> operator_map;

	void skip_until_statement_end();
	void skip_until_block_start();
	void skip_until_block_end();
	void report_error_until_member_declaration_end(const std::string& error_message);

	void report_token
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		const neon_compiler::Token& token,
		std::optional<std::string> info = std::nullopt
	);

	std::string append_ast(std::unique_ptr<neon_compiler::ast::nodes::PackageMember> node, const std::string& identifier);

	std::string get_full_identifier(const std::string& identifier) const;

	std::shared_ptr<neon_compiler::parser::OperatorTable> parse_use_statement_after_keyword_and_create_operator_table
	(
		std::shared_ptr<neon_compiler::parser::OperatorTable> previous
	);
	const std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>* parse_use_statement_after_keyword();

	std::optional<neon_compiler::ast::PackageMemberID> parse_identifier
	(
		neon_compiler::analysis::AnalysisEntryType id_type,
		neon_compiler::analysis::AnalysisSeverity id_severity
	);
	void parse_and_register_package_declaration();
	void parse_and_register_import_statement_after_keyword();
	neon_compiler::ast::nodes::Access parse_access();
	neon_compiler::ast::nodes::PackageMemberPattern parse_package_member_pattern();

	void parse_package_member
	(
		const neon_compiler::ast::nodes::Access& access,
		std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table
	);
	std::string parse_declaration_name
	(
		neon_compiler::analysis::AnalysisEntryType analysis_entry_type
	);

	void parse_operator_module_a_and_register_after_keyword(const neon_compiler::ast::nodes::Access& access);
	neon_compiler::ast::nodes::OperatorDeclaration parse_operator_declaration_after_keyword();
	void parse_operator_module_b_after_keyword(std::shared_ptr<OperatorTable> operator_table);
	neon_compiler::ast::nodes::OperatorFunction parse_operator_function
	(
		std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table
	);
	std::vector<neon_compiler::ast::nodes::OperatorFunctionPatternElement> parse_operator_function_pattern
	(
		neon_compiler::parser::OperatorTable* operator_table
	);

	std::vector<neon_compiler::ast::nodes::GenericParameter> parse_generic_parameters();
	std::vector<std::string> parse_supertype_list();

	neon_compiler::ast::nodes::ParameterDeclarationList parse_parameter_declarations_after_opening_bracket
	(
		neon_compiler::parser::OperatorTable* operator_table
	);
	std::optional<neon_compiler::ast::nodes::VariableDeclaration> parse_variable_declaration
	(
		neon_compiler::parser::OperatorTable* operator_table
	);
	std::optional<neon_compiler::ast::nodes::ReferenceType> parse_reference_type();

	std::vector<neon_compiler::ast::nodes::GenericArgument> parse_generic_arguments();
	neon_compiler::ast::nodes::CodeBlock parse_code_block_after_opening_bracket(std::shared_ptr<OperatorTable> operator_table);

	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_expression(neon_compiler::parser::OperatorTable* operator_table);
	void parse_end_of_statement_after_expression();

	std::unique_ptr<neon_compiler::ast::nodes::Statement> parse_return_statement_after_keyword(neon_compiler::parser::OperatorTable* operator_table);
	std::unique_ptr<neon_compiler::ast::nodes::Statement> parse_discard_expression(neon_compiler::parser::OperatorTable* operator_table);

	bool parse_and_register_function_or_constant
	(
		const neon_compiler::ast::nodes::Access& access,
		std::shared_ptr<OperatorTable> operator_table
	);

	bool parse_and_register_constant
	(
		const neon_compiler::ast::nodes::Access& access,
        std::shared_ptr<OperatorTable> operator_table
	);

	bool parse_and_register_function
	(
		const neon_compiler::ast::nodes::Access& access,
        std::shared_ptr<OperatorTable> operator_table
	);

	void parse_and_register_type_after_keyword
	(
		const neon_compiler::ast::nodes::Access& access,
		neon_compiler::ast::nodes::TypeAbstractionLevel abstraction_level,
		std::shared_ptr<OperatorTable> operator_table
	);
};

}

#endif // PARSER_HPP