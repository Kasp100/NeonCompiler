#include "tokeniser.hpp"

using namespace compiler;
using namespace std;

Tokeniser::Tokeniser(shared_ptr<logging::Logger> logger, unique_ptr<reading::CharReader> reader)
	: logger(logger), reader(move(reader)), tokens() {}

void Tokeniser::run()
{
	while (!reader->end_of_file_reached())
	{
		skip_whitespace();
		tokens.push_back(tokenise_next());
	}
	for(Token t : tokens)
	{
		if(t.get_lexeme().has_value()) logger->debug("Token: " + string(t.get_lexeme().value()));
	}
}

void Tokeniser::skip_whitespace()
{
	
	char c;
	bool whitespace;
	do{
		c = reader->peek();
		whitespace = is_space(c);
		if(whitespace)
		{
			reader->consume();
		}
	}while(whitespace);
}

Token Tokeniser::tokenise_next()
{
	if(is_alpha(reader->peek()))
	{
		return tokenise_identifier();
	}
}

Token Tokeniser::tokenise_identifier()
{
	std::string lexeme;
	char c;
	do
	{
		c = reader->consume();
		lexeme += c;
		c = reader->peek();
	}
	while (is_alpha(c) || is_digit(c) || c == '_');

	//TODO fix:
	return Token(TokenType::IDENTIFIER, 0, 0, optional<string_view>(lexeme));// UNSAFE as "lexeme" is local
}

bool Tokeniser::is_alpha(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool Tokeniser::is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool Tokeniser::is_space(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}