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
private:
    std::shared_ptr<logging::Logger> logger;
    std::unique_ptr<reading::CharReader> reader;
    std::vector<compiler::Token> tokens;
    compiler::Token tokenise_next();
};

}

#endif // TOKENISER_HPP