#ifndef PARSER_HPP
#define PARSER_HPP

#include <span>
#include "../token.hpp"
#include "../token_reader.hpp"
#include "../../logging/logger.hpp"

namespace neon_compiler::parser
{

class Parser
{
public:
    explicit Parser(std::span<const neon_compiler::Token> tokens, std::shared_ptr<logging::Logger> logger);
    void run();
private:
    neon_compiler::TokenReader reader;
    std::shared_ptr<logging::Logger> logger;
    void print_token(const Token& token);
};

}

#endif // PARSER_HPP