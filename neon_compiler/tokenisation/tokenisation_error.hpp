#ifndef TOKENISATION_ERROR_HPP
#define TOKENISATION_ERROR_HPP

#include <cstdint>
#include <string>

namespace tokenisation
{

class TokenisationError
{
public:
    explicit TokenisationError(std::uint32_t line, std::uint32_t column, std::string_view message);
    std::uint32_t get_line() const;
    std::uint32_t get_column() const;
    std::string_view get_message() const;
private:
    std::uint32_t line;
    std::uint32_t column;
    std::string_view message;
};

}

#endif // TOKENISATION_ERROR_HPP