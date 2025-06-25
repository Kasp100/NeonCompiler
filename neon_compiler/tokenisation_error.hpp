#ifndef TOKENISATION_ERROR_HPP
#define TOKENISATION_ERROR_HPP

#include <cstdint>
#include <string>

namespace compiler
{

class TokenisationError
{
public:
    explicit TokenisationError(std::uint32_t line, std::uint32_t column, const std::string& message);
    std::uint32_t get_line() const;
    std::uint32_t get_column() const;
    std::string get_message() const;
private:
    std::uint32_t line;
    std::uint32_t column;
    std::string message;
};

}

#endif // TOKENISATION_ERROR_HPP