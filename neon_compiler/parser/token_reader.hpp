#ifndef TOKEN_READER_HPP
#define TOKEN_READER_HPP

#include <memory>
#include <span>
#include "../token.hpp"

namespace neon_compiler::parser
{
    class TokenReader
    {
    public:
        explicit TokenReader(std::span<neon_compiler::Token> tokens);

        std::shared_ptr<neon_compiler::Token> consume(int offset = 0);
        std::shared_ptr<neon_compiler::Token> peek(int offset = 0);
        bool end_of_file_reached();
        bool consume_if_matches(const neon_compiler::TokenType& match);

    private:
        std::span<neon_compiler::Token> tokens;
        uint reading_index = 0;
    };

}

#endif // TOKEN_READER_HPP