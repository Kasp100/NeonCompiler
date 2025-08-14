#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include <optional>
#include <string>
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
			"Expected an identifier; keywords or other tokens cannot be used here.";
}

class Parser
{
public:
    explicit Parser(std::span<const neon_compiler::Token> tokens, std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter);
    void run();
private:
    neon_compiler::TokenReader reader;
    std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter;
    void report_token(neon_compiler::analysis::AnalysisEntryType type, neon_compiler::analysis::AnalyisSeverity severity,
        const neon_compiler::Token& token, std::optional<std::string> info = std::nullopt);
    std::optional<Identifier> parse_identifier(neon_compiler::analysis::AnalysisEntryType type, neon_compiler::analysis::AnalyisSeverity severity);
};

}

#endif // PARSER_HPP