#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include <optional>
#include <string>
#include "../ast/nodes/nodes.hpp"
#include "../ast/nodes/statement_nodes.hpp"
#include "../token.hpp"
#include "../token_reader.hpp"
#include "../analysis/analysis_entry.hpp"
#include "../analysis/analysis_reporter.hpp"
#include "../ast/identifiers.hpp"

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
}

namespace error_recovery
{
	constexpr std::string_view PLACEHOLDER_NAME =
		"err_name";
}

class Parser
{
public:
	explicit Parser
    (
        std::span<const neon_compiler::Token> tokens,
        std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter,
		std::shared_ptr<neon_compiler::ast::nodes::Root> root_node,
        std::string_view file
    );
	void run();
	std::shared_ptr<neon_compiler::ast::nodes::Root> get_root_node() const;
private:
	neon_compiler::TokenReader reader;
	std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter;
	std::shared_ptr<neon_compiler::ast::nodes::Root> root_node;
	std::string_view file;
	neon_compiler::ast::Identifier package{};
	std::vector<neon_compiler::ast::Identifier> imports{};

	void report_token(
			neon_compiler::analysis::AnalysisEntryType type, neon_compiler::analysis::AnalyisSeverity severity,
			const neon_compiler::Token& token, std::optional<std::string> info = std::nullopt);

	void append_ast(std::unique_ptr<neon_compiler::ast::nodes::PackageMember> node, const std::string& identifier);

	std::optional<neon_compiler::ast::Identifier> parse_identifier(neon_compiler::analysis::AnalysisEntryType type, neon_compiler::analysis::AnalyisSeverity severity);
	neon_compiler::ast::Identifier parse_expected_package_declaration();
	bool parse_optional_import_statement();
	neon_compiler::ast::nodes::Access parse_access();
	void parse_expected_package_member(const neon_compiler::ast::nodes::Access& access);
	std::string parse_expected_declaration_name(neon_compiler::analysis::AnalysisEntryType analysis_entry_type);
	void parse_expected_entrypoint(const neon_compiler::ast::nodes::Access& access);
	std::optional<neon_compiler::ast::nodes::ParemeterDeclarationList> parse_parameter_declarations();
	std::optional<neon_compiler::ast::nodes::VariableDeclaration> parse_variable_declaration(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	std::optional<neon_compiler::ast::nodes::ReferenceType> parse_reference_type(neon_compiler::ast::nodes::MutabilityMode default_mutability_mode);
	neon_compiler::ast::nodes::CodeBlock parse_expected_code_block();
	std::unique_ptr<neon_compiler::ast::nodes::Expression> parse_optional_expression();
};

}

#endif // PARSER_HPP