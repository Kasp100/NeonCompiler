#include "tokeniser.hpp"

using namespace compiler;
using namespace std;

Tokeniser::Tokeniser(shared_ptr<logging::Logger> logger, unique_ptr<reading::CharReader> reader)
    : logger(logger), reader(move(reader)), tokens() {}

void Tokeniser::run()
{
    while (!reader->end_of_file_reached())
    {
        tokens.push_back(tokenise_next());
    }
    for(Token t : tokens)
    {
        if(t.get_lexeme().has_value()) logger->debug("Token: " + string(t.get_lexeme().value()));
    }
}

Token Tokeniser::tokenise_next()
{
    reader->consume();
    return Token(TokenType::IDENTIFIER, 0, 0);
}