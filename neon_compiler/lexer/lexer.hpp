#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include "../../logging/logger.hpp"
#include "../../reading/char_reader.hpp"
#include "../token.hpp"
#include "tokenisation_error.hpp"

namespace neon_compiler::lexer
{

namespace error_messages
{
	constexpr std::string_view UNKNOWN_ESCAPE_SEQUENCE =
			"Unknown escape sequence. Valid ones are `\\n`, `\\r`, `\\t`, `\\0`, `\\\"`, `\\\'`, and `\\\\`.";
	constexpr std::string_view UNTERMINATED_STRING_LITERAL =
			"Unterminated string literal. Use double quotes (`\"`) to start and end string literals.";
	constexpr std::string_view UNTERMINATED_CHARACTER_LITERAL =
			"Unterminated character literal. Use `'` to start and end character literals.";
	constexpr std::string_view NEWLINE_IN_STRING_LITERAL =
			"Newlines in string literals are not allowed. Use `\\n` for newline characters in strings. "
			"If the literal needs splitting across multiple lines, terminate the string literal and start a new one on the next line.";
	constexpr std::string_view NEWLINE_IN_CHARACTER_LITERAL =
			"Newline in character literal is not allowed. Use `\\n` for newline characters.";
	constexpr std::string_view EMPTY_CHARACTER_LITERAL =
			"Empty character literals are not allowed. A character literal must contain exactly one character.";
	constexpr std::string_view CHARACTER_LITERAL_TOO_LONG =
			"Character literal too long. A character literal must contain exactly one character.";
	constexpr std::string_view MULTIPLE_DECIMAL_POINTS_IN_NUMBER_LITERAL =
			"Multiple decimal points in a number literal.";
	constexpr std::string_view NUMBER_BASE_PREFIX_WITHOUT_DIGITS =
			"Number base prefix (`0x`/`0b`) without digits. The number literal must have at least one digit.";
	constexpr std::string_view ILLEGAL_DIGITS_IN_NUMBER_LITERAL =
			"Illegal digits in number literal. Normal (decimal) number notation uses digits 0-9, "
			"hexadecimal number notation (prefix `0x`) uses digits 0-9 and letters A-F (upper/lower case), "
			"binary number notation (prefix `0b`) uses digits 0 and 1.";
	constexpr std::string_view DECIMAL_POINT_IN_NON_DECIMAL_LITERAL =
			"Decimal point in non decimal literal. Only base 10 number literals can have a decimal point.";
}

enum class NumberNotation
{
	BINARY,
	DECIMAL,
	HEXADECIMAL
};

class Lexer
{
public:
	explicit Lexer(std::unique_ptr<reading::CharReader> reader);
	void run();
	std::vector<Token> take_tokens();
	std::vector<TokenisationError> take_errors();
private:
	std::unique_ptr<reading::CharReader> reader;
	std::vector<neon_compiler::Token> tokens;
	std::vector<neon_compiler::lexer::TokenisationError> errors;
	void tokenise_next();
	void skip_whitespace();
	void read_and_tokenise_word();
	void tokenise_word(std::uint32_t line, std::uint32_t column, const std::string& word);
	void read_and_tokenise_number();
	void read_and_tokenise_string();
	void read_and_tokenise_character();
	std::string parse_text_literal(char opening_and_closing_char, bool merge_consecutive, std::string_view err_unterminated, std::string_view err_newline);
	void read_and_tokenise_symbol();
	void tokenise_custom_char(std::uint32_t line, std::uint32_t column, char custom_char);
	static bool is_alpha(char ch);
	static bool is_digit(NumberNotation nn, char ch);
	static bool is_space(char ch);
	static std::optional<char> convert_escaped(char ch);
	static std::optional<neon_compiler::TokenType> convert_single_char_token(char ch);
};

}

#endif // TOKENISER_HPP