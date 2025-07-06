#include "lexer.hpp"

using namespace neon_compiler::lexer;

Lexer::Lexer(std::unique_ptr<reading::CharReader> reader)
	: reader{move(reader)}, tokens{}, errors{} {}

void Lexer::run()
{
	while(!reader->end_of_file_reached())
	{
		skip_whitespace();
		tokenise_next();
	}
}

std::vector<neon_compiler::Token> Lexer::take_tokens()
{
	return std::move(tokens);
}

std::vector<TokenisationError> Lexer::take_errors()
{
	return std::move(errors);
}

void Lexer::skip_whitespace()
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

void Lexer::tokenise_next()
{
	if(reader->end_of_file_reached())
	{
		return;
	}

	if(is_alpha(reader->peek()))
	{
		read_and_tokenise_word();
		return;
	}

	if(is_digit(NumberNotation::DECIMAL, reader->peek()))
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

	read_and_tokenise_symbol();

}

void Lexer::read_and_tokenise_word()
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
	while (is_alpha(c) || is_digit(NumberNotation::DECIMAL, c) || c == '_');

	if(reader->peek() == ':')
	{
		lexeme += reader->consume();
	}

	tokenise_word(line, column, lexeme);
}

void Lexer::tokenise_word(uint32_t line, uint32_t column, const std::string& word)
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

void Lexer::read_and_tokenise_number()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
	std::string lexeme;

	NumberNotation nn = NumberNotation::DECIMAL;
	bool passed_decimal_point = false;
	bool prefix = false;

	if(reader->consume_all_if_next("0x"))
	{
		lexeme = "0x";
		nn = NumberNotation::HEXADECIMAL;
		prefix = true;
	}
	else if(reader->consume_all_if_next("0b"))
	{
		lexeme = "0b";
		nn = NumberNotation::BINARY;
		prefix = true;
	}

	if(prefix)
	{
		if(!is_digit(NumberNotation::DECIMAL, reader->peek()) && !is_alpha(reader->peek()))
		{
			errors.emplace_back(reader->get_line_number(), reader->get_column_number(), error_messages::NUMBER_BASE_PREFIX_WITHOUT_DIGITS);
		}
	}

	char c = reader->peek();

	while (is_digit(nn, c) || c == '_' || c == '.')
	{
		c = reader->consume();

		if(c == '.')
		{
			if(passed_decimal_point)
			{
				errors.emplace_back(reader->get_line_number(), reader->get_column_number(), error_messages::MULTIPLE_DECIMAL_POINTS_IN_NUMBER_LITERAL);
			}
			else
			{
				passed_decimal_point = true;
				if(nn != NumberNotation::DECIMAL)
				{
					errors.emplace_back(reader->get_line_number(), reader->get_column_number(), error_messages::DECIMAL_POINT_IN_NON_DECIMAL_LITERAL);
				}
			}
		}

		if(c != '_')
		{
			lexeme += c;
		}
		c = reader->peek();
	};

	if(is_digit(NumberNotation::DECIMAL, reader->peek()) || is_alpha(reader->peek()))
	{
		errors.emplace_back(reader->get_line_number(), reader->get_column_number(), error_messages::ILLEGAL_DIGITS_IN_NUMBER_LITERAL);
	}

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

void Lexer::read_and_tokenise_string()
{
	const uint32_t line = reader->get_line_number();
	const uint32_t column = reader->get_column_number();
	const std::string lexeme = parse_text_literal('"', true, error_messages::UNTERMINATED_STRING_LITERAL, error_messages::NEWLINE_IN_STRING_LITERAL);

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

void Lexer::read_and_tokenise_character()
{
	const uint32_t line = reader->get_line_number();
	const uint32_t column = reader->get_column_number();
	const std::string lexeme = parse_text_literal('\'', false, error_messages::UNTERMINATED_CHARACTER_LITERAL, error_messages::NEWLINE_IN_CHARACTER_LITERAL);

	if(lexeme.length() < 1)
	{
		errors.emplace_back(line, column, error_messages::EMPTY_CHARACTER_LITERAL);
	}
	else if(lexeme.length() > 1)
	{
		errors.emplace_back(line, column, error_messages::CHARACTER_LITERAL_TOO_LONG);
	}

	tokens.push_back
	(
		Token
		(
			TokenType::LITERAL_CHARACTER,
			line,
			column,
			reader->get_column_number() - column,
			std::optional<std::string>(lexeme)
		)
	);
}

std::string Lexer::parse_text_literal(char opening_and_closing_char, bool merge_consecutive, std::string_view err_unterminated, std::string_view err_newline)
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();
	std::string str = "";

	bool is_open = reader->consume() == opening_and_closing_char;

	char c = 0;
	while (is_open)
	{
		uint32_t current_column = reader->get_column_number();

		if(reader->end_of_file_reached())
		{
			errors.emplace_back(line, current_column, err_unterminated);
			break;
		}

		c = reader->consume();

		if(c == '\\')
		{
			std::optional<char> escaped = convert_escaped(reader->consume());
			if(escaped.has_value())
			{
				str += escaped.value();
			}
			else
			{
				errors.emplace_back(line, current_column, error_messages::UNKNOWN_ESCAPE_SEQUENCE);
			}
			continue;
		}

		if(c == '\n') 
		{
			line = reader->get_line_number();
			column = reader->get_column_number();
			errors.emplace_back(line, column, err_newline);
			continue;
		}

		if(c == opening_and_closing_char)
		{
			if(merge_consecutive)
			{
				skip_whitespace();
				is_open = reader->consume_if_matches(opening_and_closing_char);
			}
			else
			{
				is_open = false;
			}
			continue;
		}

		str += c;
	}

	return str;
}

void Lexer::read_and_tokenise_symbol()
{
	uint32_t line = reader->get_line_number();
	uint32_t column = reader->get_column_number();

	if(reader->consume_all_if_next("::"))
	{
		tokens.push_back(Token(TokenType::STATIC_ACCESSOR, line, column, 1, std::nullopt));
		return;
	}

	{
		std::optional<TokenType> optTokenType = convert_single_char_token(reader->peek());
		if(optTokenType.has_value())
		{
			reader->consume();
			tokens.push_back(Token(optTokenType.value(), line, column, 1, std::nullopt));
			return;
		}
	}

	char custom_char = reader->consume();
	tokenise_custom_char(line, column, custom_char);
}

void Lexer::tokenise_custom_char(uint32_t line, uint32_t column, char custom_char)
{
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

bool Lexer::is_alpha(char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}


bool Lexer::is_digit(NumberNotation nn, char ch)
{
	if(ch == '0' || ch == '1') { return true; }

	switch (nn)
	{
	case NumberNotation::DECIMAL:
		return ch >= '2' && ch <= '9';
	case NumberNotation::HEXADECIMAL:
		return (ch >= '2' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
	default:
		return false;
	}
}

bool Lexer::is_space(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

std::optional<char> Lexer::convert_escaped(char ch)
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

std::optional<neon_compiler::TokenType> Lexer::convert_single_char_token(char ch)
{
	switch (ch)
	{
		case '{': return TokenType::BRACKET_CURLY_OPEN;
		case '}': return TokenType::BRACKET_CURLY_CLOSE;
		case '(': return TokenType::BRACKET_ROUND_OPEN;
		case ')': return TokenType::BRACKET_ROUND_CLOSE;
		case '<': return TokenType::SMALLER_THAN;
		case '>': return TokenType::GREATER_THAN;
		case ';': return TokenType::END_STATEMENT;
		case ',': return TokenType::COMMA;
		default: return std::nullopt;
	}
}
