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
}

vector<Token> Tokeniser::get_tokens() const
{
	return tokens;
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
		return tokenise_word();
	}

	char custom_char = reader->consume();
	string lexeme("");
	lexeme += custom_char;
	return Token(TokenType::CUSTOM_TOKEN, 0, 0, 0, optional<string>(lexeme));
}

Token Tokeniser::tokenise_word()
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

	if(reader->peek() == ':')
	{
		lexeme += reader->consume();
	}

	return convert_word_to_token(lexeme);
}

Token Tokeniser::convert_word_to_token(const string& word)
{
	const optional<TokenType> type = Token::keyword_to_token_type(string_view(word));
	
	if(type.has_value())
	{
		return Token(*type, 0, 0, 0);
	}
	else
	{
		return Token(TokenType::IDENTIFIER, 0, 0, 0, optional<string>(word));
	}
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