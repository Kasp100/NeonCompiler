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
		tokenise_next();
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

void Tokeniser::tokenise_next()
{
	if(is_alpha(reader->peek()))
	{
		return tokenise_word();
	}

	char custom_char = reader->consume();
	string lexeme("");
	lexeme += custom_char;

	tokens.push_back
	(
		Token
		(
			TokenType::CUSTOM_TOKEN,
			reader->get_line_number(),
			reader->get_column_number(),
			1,
			optional<string>(lexeme)
		)
	);
}

void Tokeniser::tokenise_word()
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

	tokenise_word(lexeme);
}

void Tokeniser::tokenise_word(const string& word)
{
	const optional<TokenType> type = Token::keyword_to_token_type(string_view(word));
	
	if(type.has_value())
	{
		tokens.push_back
		(
			Token
			(
				*type,
				reader->get_line_number(),
				reader->get_column_number(),
				word.length()
			)
		);
	}
	else
	{
		tokens.push_back
		(
			Token
			(
				TokenType::IDENTIFIER,
				reader->get_line_number(),
				reader->get_column_number(),
				word.length(),
				optional<string>(word)
			)
		);
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