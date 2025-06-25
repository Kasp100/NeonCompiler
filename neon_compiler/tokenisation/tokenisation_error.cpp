#include "tokenisation_error.hpp"

using namespace tokenisation;

TokenisationError::TokenisationError(std::uint32_t line, std::uint32_t column, std::string_view message)
        : line(line), column(column), message(message) {}

std::uint32_t TokenisationError::get_line() const
{
    return line;
}

std::uint32_t TokenisationError::get_column() const
{
    return column;
}

std::string_view TokenisationError::get_message() const
{
    return message;
}
