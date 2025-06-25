#include "tokeniser.hpp"

using namespace tokenisation;

Tokeniser::Tokeniser(std::unique_ptr<reading::CharReader> reader)
	: reader(move(reader)), tokens(), errors() {}

void Tokeniser::run()
{
	while(!reader->end_of_file_reached())
	{
		skip_whitespace();
		tokenise_next();
	}
}

std::vector<Token> Tokeniser::get_tokens() const
{
	return tokens;
}

std::vector<TokenisationError> Tokeniser::get_errors() const
{
	return errors;
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

	if(reader->peek() == '"')
	{
		read_and_tokenise_string();
		return;
	}

	if(reader->peek() == '\'')
	{
		read_and_tokenise_character();
		return;
	}

	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();

	char custom_char = reader->consume();
	std::string lexeme("");
	lexeme += custom_char;

	tokens.push_back
	(
		Token
		(
			TokenType::CUSTOM_TOKEN,
			line,
			column,
			1,
			std::optional<std::string>(lexeme)
		)
	);
}

void Tokeniser::read_and_tokenise_word()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
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

	tokenise_word(line, column, lexeme);
}

void Tokeniser::tokenise_word(std::uint32_t line, std::uint32_t column, const std::string& word)
{
	const std::optional<TokenType> type = Token::keyword_to_token_type(std::string_view(word));
	
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
				std::optional<std::string>(word)
			)
		);
	}
}

void Tokeniser::read_and_tokenise_number()
{
	std::uint32_t line = reader->get_line_number();
	std::uint32_t column = reader->get_column_number();
	std::string lexeme;

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
			std::optional<std::string>(lexeme)
		)
	);
}

void Tokeniser::read_and_tokenise_string()
{
	std::uint32_t line = reader->get_line_number();
	std::uint32_t column = reader->get_column_number();
	std::string lexeme;

	reader->consume(); // Consume the opening quote

	bool escape_sequence = false;

	char c = reader->consume();
	while (!escape_sequence && c != '"')
	{
		uint32_t current_column = reader->get_column_number();

		if(reader->end_of_file_reached())
		{
			errors.emplace_back(line, current_column, error_messages::UNTERMINATED_STRING_LITERAL);
			break;
		}

		if(escape_sequence)
		{
			std::optional<char> escaped = convert_escaped(c);
			if(escaped.has_value())
			{
				lexeme += escaped.value();
			}
			else
			{
				errors.emplace_back(line, current_column, error_messages::UNKNOWN_ESCAPE_SEQUENCE);
			}
			
			escape_sequence = false;
			continue;
		}
		
		if(c == '\\')
		{
			escape_sequence = true;
			continue;
		}

		if(c == '\n')
		{
			line = reader->get_line_number();
			column = reader->get_column_number();
			errors.emplace_back(line, column, error_messages::NEWLINE_IN_STRING_LITERAL);
		}

		if(c == '"')
		{
			skip_whitespace();
			if(reader->consume_if_matches('"')) { break; }
		}

		lexeme += c;

		c = reader->consume();
	}

	tokens.push_back
	(
		Token
		(
			TokenType::LITERAL_STRING,
			line,
			column,
			reader->get_column_number() - column,
			std::optional<std::string>(lexeme)
		)
	);
}

void Tokeniser::read_and_tokenise_character()
{
	std::uint32_t line = reader->get_line_number();
	std::uint32_t column = reader->get_column_number();

	reader->consume();// Consume the opening quote
	char c = reader->consume();

	if(c == '\\')
	{
		c = reader->consume();

		std::optional<char> escaped = convert_escaped(c);
		if(escaped.has_value())
		{
			c = escaped.value();
		}
		else
		{
			errors.emplace_back(line, column, error_messages::UNKNOWN_ESCAPE_SEQUENCE);
		}
	}
	else if(c == '\n')
	{
		errors.emplace_back(line, column, error_messages::NEWLINE_IN_CHARACTER_LITERAL);
	}
	else if(c == '\'')
	{
		errors.emplace_back(line, column, error_messages::EMPTY_CHARACTER_LITERAL);
	}

	if(!reader->consume_if_matches('\''))
	{
		errors.emplace_back(line, column, error_messages::UNTERMINATED_CHARACTER_LITERAL);
	}

	tokens.push_back
	(
		Token
		(
			TokenType::LITERAL_CHARACTER,
			line,
			column,
			reader->get_column_number() - column,
			std::optional<std::string>("" + c)
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
	return ch == ' ' || ch == '\t' || ch == '\n';
}

std::optional<char> Tokeniser::convert_escaped(char ch)
{
	switch (ch)
	{
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '0': return '\0';
		case '"': return '"';
		case '\'': return '\'';
		case '\\': return '\\';
		default: return std::nullopt;
	}
}
