#include "parsing_error.hpp"

using namespace neon_compiler::parser;

ParsingError::ParsingError(neon_compiler::Token token, std::string_view message)
        : token{token}, message{message} {}

neon_compiler::Token ParsingError::get_token() const
{
    return token;
}

std::string_view ParsingError::get_message() const
{
    return message;
}
