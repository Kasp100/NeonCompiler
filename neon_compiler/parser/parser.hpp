#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include <optional>
#include <string>
#include "operator_table.hpp"
#include "../ast/nodes/nodes.hpp"
#include "../ast/nodes/statement_nodes.hpp"
#include "../token.hpp"
#include "../token_reader.hpp"
#include "../analysis/analysis_entry.hpp"
#include "../analysis/analysis_reporter.hpp"
#include "../ast/identifiers.hpp"
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
		"Expected a reference type followed by a name (e.g., `opt shared mut:array<int> arr`).";

	constexpr std::string_view INVALID_VARIABLE_DECLARATION =
		"Expected a variable declaration (e.g., `var int age`).";

	constexpr std::string_view INVALID_PARAMETER_DECLARATION =
		"Expected a parameter declaration (e.g., `shared mut:array<int> arr`). Separate parameters with `,` and end the parameter list with `)`.";

	constexpr std::string_view MISSING_SEMICOLON =
		"Expected `;`.";

	constexpr std::string_view MISSING_SEMICOLON_OR_FAILED_TO_PARSE_EXPRESSION =
		"Expected `;` or a valid expression. The required operator module may not be active in this scope.";

	constexpr std::string_view MISSING_CODE_BLOCK =
		"Expected a code block enclosed in `{}`.";

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

	constexpr std::string_view INVALID_OPERATOR_PARAMETER =
		"Invalid operator parameter. Operator parameters consist of a name only and do not specify a type.";

	constexpr std::string_view INVALID_SUBORDINATION =
		"Invalid subordination. Expected a non-negative integer, where `0` represents the highest precedence.";

	constexpr std::string_view INVALID_ASSOCIATIVITY =
		"Invalid associativity. Expected `left`, `right`, or no associativity.";

	constexpr std::string_view INVALID_OPERATOR_PROPERTY =
		"Invalid operator property. Supported properties are `subordination` (precedence) and `associativity`.";

	constexpr std::string_view INVALID_USE_STATEMENT_ARGUMENT =
		"Invalid `use` statement. Expected a reference to an operator module.";
}

namespace error_recovery
{
	constexpr std::string_view PLACEHOLDER_NAME =
		"err_name";
	constexpr std::string_view PLACEHOLDER_TYPE =
		"err_type";
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
    ) :
		logger{init_logger},
		reader{init_tokens},
		analysis_reporter{init_analysis_reporter},
		root_node{init_root_node},
		file{init_file},
		operator_map{init_operator_map}
	{}

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
	neon_compiler::ast::Identifier package;

	/** Mapping from reference name to declaration path */
	std::unordered_map<std::string, std::string> imports;
	/** Mapping from package member identifier to operator lists */
	std::shared_ptr<OperatorMap> operator_map;

	void skip_until_statement_end();
	void skip_until_block_start();
	void skip_until_block_end();

	void report_token
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity,
		const neon_compiler::Token& token,
		std::optional<std::string> info = std::nullopt
	);

	std::string append_ast(std::unique_ptr<neon_compiler::ast::nodes::PackageMember> node, const std::string& identifier);

	/* Names:
	 * **with** "expected": a check is done to see if what the reader sees is the expected thing
	 * **without** "expected": no check is done and assumes what the reader sees is the (optional/required) thing
	 */

	std::shared_ptr<neon_compiler::parser::OperatorTable> parse_use_statement_and_create_operator_table
	(
		std::shared_ptr<neon_compiler::parser::OperatorTable> previous
	);
	const std::vector<std::shared_ptr<const neon_compiler::parser::Operator>>* parse_use_statement();

	std::optional<neon_compiler::ast::Identifier> parse_identifier
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity
	);
	void parse_and_register_expected_package_declaration();
	void parse_and_register_import_statement();
	neon_compiler::ast::nodes::Access parse_access();
	neon_compiler::ast::nodes::PackageMemberPattern parse_package_member_pattern();

	void parse_expected_package_member
	(
		const neon_compiler::ast::nodes::Access& access,
		std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table
	);
	std::string parse_expected_declaration_name
	(
		neon_compiler::analysis::AnalysisEntryType analysis_entry_type
	);
	void parse_and_register_expected_entrypoint
	(
		const neon_compiler::ast::nodes::Access& access,
		std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table
	);

	void parse_expected_operator_module_a_and_register(const neon_compiler::ast::nodes::Access& access);
	neon_compiler::ast::nodes::OperatorDeclaration parse_expected_operator_declaration();
	void parse_expected_operator_module_b(std::shared_ptr<OperatorTable> operator_table);
	neon_compiler::ast::nodes::OperatorFunction parse_expected_operator_function(std::shared_ptr<neon_compiler::parser::OperatorTable> operator_table);
	std::vector<neon_compiler::ast::nodes::OperatorFunctionPatternElement> parse_operator_function_pattern();

	neon_compiler::ast::nodes::ParameterDeclarationList parse_parameter_declarations();
	std::optional<neon_compiler::ast::nodes::VariableDeclaration> parse_variable_declaration(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	std::optional<neon_compiler::ast::nodes::ReferenceType> parse_reference_type(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	neon_compiler::ast::nodes::CodeBlock parse_code_block_until_end(std::shared_ptr<OperatorTable> operator_table);
	std::unique_ptr<neon_compiler::ast::nodes::Statement> parse_return_statement(neon_compiler::parser::OperatorTable* operator_table);
	std::unique_ptr<neon_compiler::ast::nodes::Statement> parse_expected_discard_expression(neon_compiler::parser::OperatorTable* operator_table);
};

}

#endif // PARSER_HPP