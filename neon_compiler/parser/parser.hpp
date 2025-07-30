#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include <optional>
#include <string>
#include "../token.hpp"
#include "../token_reader.hpp"
#include "parsing_error.hpp"
#include "../../logging/logger.hpp"
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
    explicit Parser(std::span<const neon_compiler::Token> tokens, std::shared_ptr<logging::Logger> logger);
    void run();
	std::vector<neon_compiler::parser::ParsingError> take_errors();
private:
    neon_compiler::TokenReader reader;
    std::vector<neon_compiler::parser::ParsingError> errors;
    std::shared_ptr<logging::Logger> logger;
    void print_token(const Token& token);
    std::optional<Identifier> parse_identifier();
};

}

#endif // PARSER_HPP