#ifndef PARSING_ERROR_HPP
#define PARSING_ERROR_HPP

#include <string_view>
#include "../token.hpp"

namespace neon_compiler::parser
{

class ParsingError
{
public:
    explicit ParsingError(neon_compiler::Token token, std::string_view message);
    neon_compiler::Token get_token() const;
    std::string_view get_message() const;
private:
    neon_compiler::Token token;
    std::string_view message;
};

}

#endif // PARSING_ERROR_HPP