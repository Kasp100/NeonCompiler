#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include "../token.hpp"

namespace neon_compiler::parser
{

class Parser
{
public:
    explicit Parser(std::span<neon_compiler::Token> tokens);
private:
    std::span<neon_compiler::Token> tokens;
};

}

#endif // PARSER_HPP