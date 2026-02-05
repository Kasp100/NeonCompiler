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
		*TEST_LITERAL_NUMBER_DECIMAL = "105_788",
		*TEST_LITERAL_NUMBER_DECIMAL_FLOAT = "105_788.750_1",
		*TEST_LITERAL_NUMBER_HEXADECIMAL = "0x758a_0b71",
		*TEST_LITERAL_NUMBER_BINARY = "0b10110001_00101110",
		*TEST_ILLEGAL_LITERAL_STRING = "\"a\\qb\nc",
		*TEST_ILLEGAL_LITERAL_CHARACTER = "'ab'",
		*TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_HEXADECIMAL = "0x012345689ABCDEFG",
		*TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_BINARY = "0b012",
		*TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_DECIMAL = "0123456789AG",
		*TEST_ILLEGAL_LITERAL_NUMBER_PREFIX_WITHOUT_DIGITS_HEXADECIMAL = "0x",
		*TEST_ILLEGAL_LITERAL_NUMBER_PREFIX_WITHOUT_DIGITS_BINARY = "0b",
		*TEST_ILLEGAL_LITERAL_NUMBER_MULTIPLE_DECIMAL_POINTS = "70.1.7",
		*TEST_ILLEGAL_LITERAL_NUMBER_DECIMAL_POINT_IN_NON_DECIMAL_LITERAL = "0xabc.def 0b1010.101001";

TEST_CASE("Keywords are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_KEYWORDS);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 5);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::PACKAGE);
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::PACKAGE_MEMBER_INTERFACE);
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::MUT);
	CHECK(tokens[3].get_type() == neon_compiler::TokenType::COLON);
	CHECK(tokens[4].get_type() == neon_compiler::TokenType::END_OF_FILE);
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
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 4);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_STRING);
	CHECK(tokens[0].get_lexeme().value() == "strings");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::COMMA);
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::LITERAL_STRING);
	CHECK(tokens[2].get_lexeme().value() == "testing");
	CHECK(tokens[3].get_type() == neon_compiler::TokenType::END_OF_FILE);
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
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 4);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[0].get_lexeme().value() == "c");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[1].get_lexeme().value() == "'");
	CHECK(tokens[2].get_type() == neon_compiler::TokenType::LITERAL_CHARACTER);
	CHECK(tokens[2].get_lexeme().value() == "\n");
	CHECK(tokens[3].get_type() == neon_compiler::TokenType::END_OF_FILE);
}

TEST_CASE("Decimal number literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_NUMBER_DECIMAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 2);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_NUMBER);
	CHECK(tokens[0].get_lexeme().value() == "105788");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::END_OF_FILE);
}

TEST_CASE("Floating point decimal number literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_NUMBER_DECIMAL_FLOAT);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 2);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_NUMBER);
	CHECK(tokens[0].get_lexeme().value() == "105788.7501");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::END_OF_FILE);
}

TEST_CASE("Hexadecimal number literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_NUMBER_HEXADECIMAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 2);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_NUMBER);
	CHECK(tokens[0].get_lexeme().value() == "0x758a0b71");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::END_OF_FILE);
}

TEST_CASE("Binary number literals are parsed correctly")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_LITERAL_NUMBER_BINARY);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	CHECK(lexer.take_errors().size() == 0);

	const std::vector<neon_compiler::Token> tokens = lexer.take_tokens();
	CHECK(tokens.size() == 2);

	CHECK(tokens[0].get_type() == neon_compiler::TokenType::LITERAL_NUMBER);
	CHECK(tokens[0].get_lexeme().value() == "0b1011000100101110");
	CHECK(tokens[1].get_type() == neon_compiler::TokenType::END_OF_FILE);
}

TEST_CASE("Illegal string literals are disallowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_STRING);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 3);

	CHECK(errors[0].message == error_messages::UNKNOWN_ESCAPE_SEQUENCE);
	CHECK(errors[1].message == error_messages::NEWLINE_IN_STRING_LITERAL);
	CHECK(errors[2].message == error_messages::UNTERMINATED_STRING_LITERAL);
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
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::CHARACTER_LITERAL_TOO_LONG);
}

TEST_CASE("Illegal hexadecimal digits aren't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_HEXADECIMAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::ILLEGAL_DIGITS_IN_NUMBER_LITERAL);
}

TEST_CASE("Illegal binary digits aren't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_BINARY);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::ILLEGAL_DIGITS_IN_NUMBER_LITERAL);
}

TEST_CASE("Illegal hexadecimal digits aren't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_ILLEGAL_DIGITS_DECIMAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::ILLEGAL_DIGITS_IN_NUMBER_LITERAL);
}

TEST_CASE("Hexadecimal prefix without digits isn't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_PREFIX_WITHOUT_DIGITS_HEXADECIMAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::NUMBER_BASE_PREFIX_WITHOUT_DIGITS);
}

TEST_CASE("Binary prefix without digits isn't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_PREFIX_WITHOUT_DIGITS_BINARY);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::NUMBER_BASE_PREFIX_WITHOUT_DIGITS);
}

TEST_CASE("Multiple decimal points aren't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_MULTIPLE_DECIMAL_POINTS);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 1);

	CHECK(errors[0].message == error_messages::MULTIPLE_DECIMAL_POINTS_IN_NUMBER_LITERAL);
}

TEST_CASE("Decimal points in non decimal literals aren't allowed")
{
	// Arrange
	std::unique_ptr<std::istringstream> iss = std::make_unique<std::istringstream>(TEST_ILLEGAL_LITERAL_NUMBER_DECIMAL_POINT_IN_NON_DECIMAL_LITERAL);
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(std::move(iss));

	// Act
	Lexer lexer{std::move(reader)};
	lexer.run();

	// Assert
	const std::vector<neon_compiler::lexer::TokenisationError> errors = lexer.take_errors();
	CHECK(errors.size() == 2);

	CHECK(errors[0].message == error_messages::DECIMAL_POINT_IN_NON_DECIMAL_LITERAL);
	CHECK(errors[1].message == error_messages::DECIMAL_POINT_IN_NON_DECIMAL_LITERAL);
}
