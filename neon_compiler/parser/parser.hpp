#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include <vector>
#include "../token.hpp"
#include "../token_reader.hpp"
#include "parsing_error.hpp"
#include "../../logging/logger.hpp"

namespace neon_compiler::parser
{

namespace error_messages
{
	constexpr std::string_view MISSING_PACKAGE_DECLARATION =
			"Expecting a package declaration. Example: `pkg main::example;`";
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
};

}

#endif // PARSER_HPP