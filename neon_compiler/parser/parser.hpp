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
		"Unexpected end of file: a terminating token is missing. "
		"Statements are terminated with `;` and blocks with `}`.";
	constexpr std::string_view MISSING_PACKAGE_DECLARATION =
		"Expected a package declaration. Example: `pkg main::example;`";
	constexpr std::string_view MISSING_IDENTIFIER =
		"Expected an identifier. Keywords or other tokens cannot be used here.";
	constexpr std::string_view PROTECTED_PACKAGE_MEMBER =
		"Keyword `protected` cannot be used for package members (e.g., classes), only for type members.";
	constexpr std::string_view INVALID_FILE_LEVEL_TOKEN =
		"Invalid file level token. See documentation.";
	constexpr std::string_view INVALID_IMPORT_STATEMENT =
		"Expected a package member reference (e.g. `my_package::my_class`) in import statement.";
	constexpr std::string_view INVALID_DECLARATION_NAME =
		"Invalid name for this declaration. Keywords or other tokens cannot be used here.";
	constexpr std::string_view INVALID_REFERENCE_TYPE =
		"Expected a reference type and type name, e.g. `opt shared mut:array<int> arr`.";
	constexpr std::string_view INVALID_VARIABLE_DECLARATION =
		"Expected a variable declaration, e.g. `var int age`.";
	constexpr std::string_view INVALID_PARAMETER_DECLARATION =
		"Expected a parameter declaration, e.g. `shared mut:array<int> arr`. Terminate with `)` and separate parameter declarations with `,`.";
	constexpr std::string_view MISSING_SEMICOLON =
		"Missing a semicolon.";
	constexpr std::string_view MISSING_CODE_BLOCK =
		"Expected a code block, starting with `{` and ending with `}`.";
	constexpr std::string_view MISSING_SECOND_PACKAGE_MEMBER_PATTERN =
		"Expected a second package member pattern here.";
	constexpr std::string_view MISSING_PACKAGE_MEMBER_PATTERNS =
		"Expected package member patterns, starting with `{` and ending with `}`.";
	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_CLOSING_BRACKET =
		"Invalid package member pattern, expected `}`.";
	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_PKG =
		"Invalid package member pattern, expected `pkg` after `shallow` or `deep`.";
	constexpr std::string_view INVALID_PACKAGE_MEMBER_PATTERN_PART__EXPECTED_CLOSING_BRACKET =
		"Expected the parameter declaration to end here with `)`.";
	constexpr std::string_view INVALID_OPERATOR_PARAMETER =
		"Invalid operator parameter - operator parameters have a name but no type in the operator declaration.";
	constexpr std::string_view INVALID_NAMED_EXPRESSION =
		"Invalid named expression. The dot operator expects a named expression after the dot, e.g. `method_name()` or `field_name`.";
	constexpr std::string_view INVALID_SUBORDINATION =
		"Invalid subordination; must be a natural number. 0 is the lowest subordination / highest precedence.";
	constexpr std::string_view INVALID_ASSOCIATIVITY =
		"Invalid associativity; must be `left` or `right`, or not defined.";
	constexpr std::string_view INVALID_OPERATOR_PROPERTY =
		"Invalid operator property. Only `subordination` (defines precedence) and `associativity` are supported.";
}

namespace error_recovery
{
	constexpr std::string_view PLACEHOLDER_NAME =
		"err_name";
	constexpr std::string_view PLACEHOLDER_TYPE =
		"err_type";
}

using OperatorMap = std::unordered_map<std::string, std::vector<neon_compiler::parser::Operator>>;

class Parser
{
public:
	explicit Parser
    (
		std::shared_ptr<logging::Logger> logger,
        std::span<const neon_compiler::Token> tokens,
        std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter,
		std::shared_ptr<neon_compiler::ast::nodes::Root> root_node,
        std::string_view file,
		std::shared_ptr<OperatorMap> operator_map,
		neon_compiler::parser::OperatorTable* operator_table
    ) :
		logger{logger},
		reader{tokens},
		analysis_reporter{analysis_reporter},
		root_node{root_node},
		file{file},
		operator_map{operator_map},
		operator_table{operator_table} {}

	/** Should be run first in the parsing phase to register package declaration and parse operator modules. */
	void run_a();

	/** Should be run second in the parsing phase to parse other package member types. */
	void run_b();

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
	/** Global operator table */
	neon_compiler::parser::OperatorTable* operator_table;

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

	void parse_use_statement();

	std::optional<neon_compiler::ast::Identifier> parse_identifier
	(
		neon_compiler::analysis::AnalysisEntryType type,
		neon_compiler::analysis::AnalysisSeverity severity
	);
	void parse_and_register_expected_package_declaration();
	void parse_and_register_import_statement();
	neon_compiler::ast::nodes::Access parse_access();
	neon_compiler::ast::nodes::PackageMemberPattern parse_package_member_pattern();
	void parse_expected_package_member(const neon_compiler::ast::nodes::Access& access);
	std::string parse_expected_declaration_name(neon_compiler::analysis::AnalysisEntryType analysis_entry_type);
	void parse_and_register_expected_entrypoint(const neon_compiler::ast::nodes::Access& access);

	void parse_expected_operator_module_a_and_register(const neon_compiler::ast::nodes::Access& access);
	neon_compiler::ast::nodes::OperatorDeclaration parse_expected_operator_declaration();
	void parse_expected_operator_module_b();
	neon_compiler::ast::nodes::OperatorFunction parse_expected_operator_function();
	std::vector<neon_compiler::ast::nodes::OperatorFunctionPatternElement> parse_operator_function_pattern();

	neon_compiler::ast::nodes::ParameterDeclarationList parse_parameter_declarations();
	std::optional<neon_compiler::ast::nodes::VariableDeclaration> parse_variable_declaration(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	std::optional<neon_compiler::ast::nodes::ReferenceType> parse_reference_type(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	neon_compiler::ast::nodes::CodeBlock parse_code_block_until_end();
	std::unique_ptr<neon_compiler::ast::nodes::Statement> parse_return_statement();
};

}

#endif // PARSER_HPP