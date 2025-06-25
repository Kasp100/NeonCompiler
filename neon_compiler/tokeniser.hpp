#ifndef TOKENISER_HPP
#define TOKENISER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include "../logging/logger.hpp"
#include "../reading/char_reader.hpp"
#include "token.hpp"
#include "tokenisation_error.hpp"

namespace compiler
{

namespace error_messages
{
	constexpr std::string_view UNKNOWN_ESCAPE_SEQUENCE =
			"Unknown escape sequence. Valid escape sequences are: \\n, \\r, \\t, \\0, \\\", \\\', and \\\\.";
	constexpr std::string_view UNTERMINATED_STRING_LITERAL =
			"Unterminated string literal. Use double quotes to start and end string literals.";
	constexpr std::string_view UNTERMINATED_CHARACTER_LITERAL =
			"Unterminated character literal. Use single quotes to start and end character literals.";
	constexpr std::string_view NEWLINE_IN_STRING_LITERAL =
			"Newline in string literal is not allowed. "
			"Use \\n for newline characters, or terminate the string literal and restart on the next line for readability.";
	constexpr std::string_view NEWLINE_IN_CHARACTER_LITERAL =
			"Newline in character literal is not allowed. Use \\n for newline characters.";
	constexpr std::string_view EMPTY_CHARACTER_LITERAL =
			"Empty character literals are not allowed. A character literal must contain exactly one character.";
}

class Tokeniser
{
public:
	explicit Tokeniser(std::unique_ptr<reading::CharReader> reader);
	void run();
	std::vector<compiler::Token> get_tokens() const;
	std::vector<compiler::TokenisationError> get_errors() const;
private:
	std::unique_ptr<reading::CharReader> reader;
	std::vector<compiler::Token> tokens;
	std::vector<compiler::TokenisationError> errors;
	void tokenise_next();
	void skip_whitespace();
	void read_and_tokenise_word();
	void tokenise_word(std::uint32_t line, std::uint32_t column, const std::string& word);
	void read_and_tokenise_number();
	void read_and_tokenise_string();
	void read_and_tokenise_character();
	static bool is_alpha(char ch);
	static bool is_digit(char ch);
	static bool is_space(char ch);
	static std::optional<char> convert_escaped(char ch);
};

}

#endif // TOKENISER_HPP