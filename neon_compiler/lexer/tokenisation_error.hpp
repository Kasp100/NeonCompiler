#ifndef TOKENISATION_ERROR_HPP
#define TOKENISATION_ERROR_HPP

#include <cstdint>
#include <string>

namespace tokenisation
{

class TokenisationError
{
public:
    explicit TokenisationError(uint32_t line, uint32_t column, std::string_view message);
    uint32_t get_line() const;
    uint32_t get_column() const;
    std::string_view get_message() const;
private:
    uint32_t line;
    uint32_t column;
    std::string_view message;
};

}

#endif // TOKENISATION_ERROR_HPP