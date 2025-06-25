#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include "../../logging/logger.hpp"
#include "../../reading/char_reader.hpp"
#include "token.hpp"
#include "tokenisation_error.hpp"

namespace tokenisation
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
			"A character literal must contain exactly one character.";
}

class Tokeniser
{
public:
	explicit Tokeniser(std::unique_ptr<reading::CharReader> reader);
	void run();
	std::vector<tokenisation::Token> get_tokens() const;
	std::vector<tokenisation::TokenisationError> get_errors() const;
private:
	std::unique_ptr<reading::CharReader> reader;
	std::vector<tokenisation::Token> tokens;
	std::vector<tokenisation::TokenisationError> errors;
	void tokenise_next();
	void skip_whitespace();
	void read_and_tokenise_word();
	void tokenise_word(std::uint32_t line, std::uint32_t column, const std::string& word);
	void read_and_tokenise_number();
	void read_and_tokenise_string();
	void read_and_tokenise_character();
	void read_and_tokenise_symbol();
	void tokenise_custom_char(std::uint32_t line, std::uint32_t column, char custom_char);
	static bool is_alpha(char ch);
	static bool is_digit(char ch);
	static bool is_space(char ch);
	static std::optional<char> convert_escaped(char ch);
	static std::optional<tokenisation::TokenType> convert_single_char_token(char ch);
};

}

#endif // TOKENISER_HPP