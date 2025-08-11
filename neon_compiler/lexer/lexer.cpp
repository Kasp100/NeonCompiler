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

	tokens.emplace_back
	(
		TokenType::END_OF_FILE,
		reader->get_source_position(),
		0
	);
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
	unsigned char c;
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
	while (whitespace && !reader->end_of_file_reached());
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
	reading::SourcePosition sp = reader->get_source_position();
	std::string lexeme;

	unsigned char c;
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

	tokenise_word(sp, lexeme);
}

void Lexer::tokenise_word(reading::SourcePosition sp, const std::string& word)
{
	const std::optional<TokenType> type = Token::keyword_to_token_type(std::string_view(word));
	
	if (type.has_value())
	{
		tokens.emplace_back
		(
			*type,
			sp,
			word.length()
		);
	}
	else
	{
		tokens.emplace_back
		(
			TokenType::IDENTIFIER,
			sp,
			word.length(),
			std::optional<std::string>(word)
		);
	}
}

void Lexer::read_and_tokenise_number()
{
	reading::SourcePosition sp = reader->get_source_position();
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
			errors.emplace_back(reader->get_source_position(), error_messages::NUMBER_BASE_PREFIX_WITHOUT_DIGITS);
		}
	}

	unsigned char c = reader->peek();

	while (is_digit(nn, c) || c == '_' || c == '.')
	{
		c = reader->consume();

		if(c == '.')
		{
			if(passed_decimal_point)
			{
				errors.emplace_back(reader->get_source_position(), error_messages::MULTIPLE_DECIMAL_POINTS_IN_NUMBER_LITERAL);
			}
			else
			{
				passed_decimal_point = true;
				if(nn != NumberNotation::DECIMAL)
				{
					errors.emplace_back(reader->get_source_position(), error_messages::DECIMAL_POINT_IN_NON_DECIMAL_LITERAL);
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
		errors.emplace_back(reader->get_source_position(), error_messages::ILLEGAL_DIGITS_IN_NUMBER_LITERAL);
	}

	tokens.emplace_back
	(
		TokenType::LITERAL_NUMBER,
		sp,
		lexeme.length(),
		std::optional<std::string>(lexeme)
	);
}

void Lexer::read_and_tokenise_string()
{
	reading::SourcePosition sp = reader->get_source_position();
	const uint32_t column = sp.offset_in_line;
	const std::string lexeme = parse_text_literal('"', true, error_messages::UNTERMINATED_STRING_LITERAL, error_messages::NEWLINE_IN_STRING_LITERAL);

	tokens.emplace_back
	(
		TokenType::LITERAL_STRING,
		sp,
		reader->get_source_position().offset_in_line - column,
		std::optional<std::string>(lexeme)
	);
}

void Lexer::read_and_tokenise_character()
{
	reading::SourcePosition sp = reader->get_source_position();
	const uint32_t column = sp.offset_in_line;
	const std::string lexeme = parse_text_literal('\'', false, error_messages::UNTERMINATED_CHARACTER_LITERAL, error_messages::NEWLINE_IN_CHARACTER_LITERAL);

	if(lexeme.length() < 1)
	{
		errors.emplace_back(sp, error_messages::EMPTY_CHARACTER_LITERAL);
	}
	else if(lexeme.length() > 1)
	{
		errors.emplace_back(sp, error_messages::CHARACTER_LITERAL_TOO_LONG);
	}

	tokens.emplace_back
	(
		TokenType::LITERAL_CHARACTER,
		sp,
		reader->get_source_position().offset_in_line - column,
		std::optional<std::string>(lexeme)
	);
}

std::string Lexer::parse_text_literal(unsigned char opening_and_closing_char, bool merge_consecutive, std::string_view err_unterminated, std::string_view err_newline)
{
	std::string str = "";

	bool is_open = reader->consume() == opening_and_closing_char;

	unsigned char c = 0;
	while(is_open)
	{
		if(reader->end_of_file_reached())
		{
			errors.emplace_back(reader->get_source_position(), err_unterminated);
			break;
		}

		c = reader->consume();
		reading::SourcePosition sp = reader->get_source_position();

		if(c == '\\')
		{
			std::optional<char> escaped = convert_escaped(reader->consume());
			if(escaped.has_value())
			{
				str += escaped.value();
			}
			else
			{
				errors.emplace_back(sp, error_messages::UNKNOWN_ESCAPE_SEQUENCE);
			}
			continue;
		}

		if(c == '\n') 
		{
			errors.emplace_back(sp, err_newline);
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
	reading::SourcePosition sp = reader->get_source_position();

	if(reader->consume_all_if_next("::"))
	{
		tokens.emplace_back(TokenType::STATIC_ACCESSOR, sp, 2, std::nullopt);
		return;
	}

	{
		std::optional<TokenType> optTokenType = convert_single_char_token(reader->peek());
		if(optTokenType.has_value())
		{
			reader->consume();
			tokens.emplace_back(optTokenType.value(), sp, 1, std::nullopt);
			return;
		}
	}

	char custom_char = reader->consume();
	tokenise_custom_char(sp, custom_char);
}

void Lexer::tokenise_custom_char(reading::SourcePosition sp, unsigned char custom_char)
{
	std::string lexeme("");
	lexeme += custom_char;

	tokens.emplace_back
	(
		TokenType::CUSTOM_TOKEN,
		sp,
		1,
		std::optional<std::string>(lexeme)
	);
}

bool Lexer::is_alpha(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}


bool Lexer::is_digit(NumberNotation nn, unsigned char ch)
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

bool Lexer::is_space(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

std::optional<unsigned char> Lexer::convert_escaped(unsigned char ch)
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

std::optional<neon_compiler::TokenType> Lexer::convert_single_char_token(unsigned char ch)
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
