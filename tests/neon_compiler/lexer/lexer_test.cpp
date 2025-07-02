#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libs/doctest/doctest.hpp"

#include <memory>
#include <span>
#include "../../../neon_compiler/token.hpp"
#include "../../../neon_compiler/lexer/lexer.hpp"
#include "../../../reading/char_reader.hpp"

using namespace neon_compiler::lexer;

constexpr const char
		*TEST_KEYWORDS = "pkg interface mut:",
		*TEST_LITERAL_STRING = "\"strings\",\"test\" \"ing\"",
		*TEST_LITERAL_CHARACTER = "'c' '\\'' '\\n'",
		*TEST_ILLEGAL_LITERAL_CHARACTER = "'ab'";

TEST_CASE("Keywords are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_KEYWORDS);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.get_errors().size() == 0);

	std::span<const neon_compiler::Token> tokens = lexer.get_tokens();
	CHECK(tokens.size() == 3);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::PACKAGE);
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::TYPE_INTERFACE);
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::MUTABLE_REFERENCE);
}

TEST_CASE("String literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_STRING);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.get_errors().size() == 0);

	std::span<const neon_compiler::Token> tokens = lexer.get_tokens();
	CHECK(tokens.size() == 3);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_STRING);
	CHECK(tokens[0].get_lexeme().value() == "strings");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::COMMA);
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::LITERAL_STRING);
	CHECK(tokens[2].get_lexeme().value() == "testing");
}

TEST_CASE("Character literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_CHARACTER);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.get_errors().size() == 0);

	std::span<const neon_compiler::Token> tokens = lexer.get_tokens();
	CHECK(tokens.size() == 3);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[0].get_lexeme().value() == "c");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[1].get_lexeme().value() == "'");
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[2].get_lexeme().value() == "\n");
}


TEST_CASE("Illegal character literals are disallowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_CHARACTER);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	std::span<const neon_compiler::lexer::TokenisationError> errors = lexer.get_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].get_message() == error_messages::CHARACTER_LITERAL_TOO_LONG);
}

