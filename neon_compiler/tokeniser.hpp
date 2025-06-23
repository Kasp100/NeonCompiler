#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <memory>
#include "../logging/logger.hpp"
#include "../reading/char_reader.hpp"
#include "token.hpp"

namespace compiler
{

class Tokeniser
{
public:
    explicit Tokeniser(std::shared_ptr<logging::Logger> logger, std::unique_ptr<reading::CharReader> reader);
    void run();
    std::vector<compiler::Token> get_tokens() const;
private:
    std::shared_ptr<logging::Logger> logger;
    std::unique_ptr<reading::CharReader> reader;
    std::vector<compiler::Token> tokens;
    void tokenise_next();
    void skip_whitespace();
    void tokenise_word();
    void tokenise_number();
    void tokenise_word(const std::string& word);
    static bool is_alpha(char ch);
    static bool is_digit(char ch);
    static bool is_space(char ch);
};

}

#endif // TOKENISER_HPP