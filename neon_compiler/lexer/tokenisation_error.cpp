#include "tokenisation_error.hpp"

using namespace neon_compiler::lexer;

TokenisationError::TokenisationError(uint32_t line, uint32_t column, std::string_view message)
        : line{line}, column{column}, message{message} {}

uint32_t TokenisationError::get_line() const
{
    return line;
}

uint32_t TokenisationError::get_column() const
{
    return column;
}

std::string_view TokenisationError::get_message() const
{
    return message;
}
