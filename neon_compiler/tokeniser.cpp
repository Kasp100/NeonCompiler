#include "tokeniser.hpp"

using namespace compiler;
using namespace std;

Tokeniser::Tokeniser(shared_ptr<logging::Logger> logger, unique_ptr<reading::CharReader> reader)
	: logger(logger), reader(move(reader)), tokens() {}

void Tokeniser::run()
{
	while(!reader->end_of_file_reached())
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
	do
	{
		c = reader->peek();
		whitespace = is_space(c);
		if (whitespace)
		{
			reader->consume();
		}
	}
	while (whitespace);
}

void Tokeniser::tokenise_next()
{
	if(is_alpha(reader->peek()))
	{
		read_and_tokenise_word();
		return;
	}

	if(is_digit(reader->peek()))
	{
		read_and_tokenise_number();
		return;
	}

	if(reader->consume_if_matches('"'))
	{
		read_and_tokenise_string();
		return;
	}

	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();

	char custom_char = reader->consume();
	string lexeme("");
	lexeme += custom_char;

	tokens.push_back
	(
		Token
		(
			TokenType::CUSTOM_TOKEN,
			line,
			column,
			1,
			optional<string>(lexeme)
		)
	);
}

void Tokeniser::read_and_tokenise_word()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
	string lexeme;

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

	tokenise_word(line, column, lexeme);
}

void Tokeniser::tokenise_word(uint32_t line, uint32_t column, const string& word)
{
	const optional<TokenType> type = Token::keyword_to_token_type(string_view(word));
	
	if (type.has_value())
	{
		tokens.push_back
		(
			Token
			(
				*type,
				line,
				column,
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
				line,
				column,
				word.length(),
				optional<string>(word)
			)
		);
	}
}

void Tokeniser::read_and_tokenise_number()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
	string lexeme;

	if(reader->consume_all_if_next("0x"))
	{
		lexeme = "0x";
	}
	else if(reader->consume_all_if_next("0b"))
	{
		lexeme = "0b";
	}

	char c = reader->peek();

	/* 1. `is_digit` to check for decimal digits
	 * 2. `is_alpha` to check for hexadecimal digits (A-F, a-f)
	 * 3. `.` is allowed in floating point numbers
	 * 4. `_` is allowed as a separator in numbers, for readability (e.g., 1_000_000)
	 * Validation of numbers is done in the parser. */
	while (is_digit(c) || is_alpha(c) || c == '.' || c == '_')
	{
		c = reader->consume();
		lexeme += c;
		c = reader->peek();
	};

	tokens.push_back
	(
		Token
		(
			TokenType::LITERAL_NUMBER,
			line,
			column,
			lexeme.length(),
			optional<string>(lexeme)
		)
	);
}

void Tokeniser::read_and_tokenise_string()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
	string lexeme;

	bool escape_sequence = false;

	char c = reader->consume();
	bool ending_at_eof;
	while (!(ending_at_eof = reader->end_of_file_reached()))
	{
		if(escape_sequence)
		{
			optional<char> escaped = convert_escaped(c);
			if(escaped.has_value())
			{
				lexeme += escaped.value();
			}
			else
			{
				logger->error("Unknown escape sequence at line " + to_string(line) + ", column " + to_string(column));
				return;
			}
			
			escape_sequence = false;
			continue;
		}
		
		if(c == '\\')
		{
			escape_sequence = true;
			continue;
		}
		
		if(c == '"')
		{
			break;
		}

		lexeme += c;

		c = reader->consume();
	}

	if(ending_at_eof)
	{
		logger->error("Unterminated string literal at line " + to_string(line) + ", column " + to_string(column));
		return;
	}

	tokens.push_back
	(
		Token
		(
			TokenType::LITERAL_STRING,
			line,
			column,
			lexeme.length(),
			optional<string>(lexeme)
		)
	);
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

optional<char> Tokeniser::convert_escaped(char ch)
{
	switch (ch) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '0': return '\0';
		case '"': return '"';
		case '\'': return '\'';
		case '\\': return '\\';
		default: return nullopt;
	}
}
