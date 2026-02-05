#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libs/doctest/doctest.hpp"

#include <vector>
#include "../../../neon_compiler/token_reader.hpp"

using namespace neon_compiler;

static const std::vector<Token> TEST_TOKENS
{
	Token{TokenType::PACKAGE, reading::SourcePosition{0, 0, 0}, 0},
	Token{TokenType::IDENTIFIER, reading::SourcePosition{0, 0, 0}, 0},
	Token{TokenType::END_STATEMENT, reading::SourcePosition{0, 0, 0}, 0},
	Token{TokenType::IDENTIFIER, reading::SourcePosition{0, 0, 0}, 0},
	Token{TokenType::END_OF_FILE, reading::SourcePosition{0, 0, 0}, 0}
};

TEST_CASE("Token reader works")
{
	// Arrange
	std::span<const Token> tokens = std::span{TEST_TOKENS};
	TokenReader tr{tokens};

	// Act & Assert
	CHECK(tr.peek().get_type() == TokenType::PACKAGE);
	CHECK(tr.peek(1).get_type() == TokenType::IDENTIFIER);
	CHECK(tr.peek(2).get_type() == TokenType::END_STATEMENT);
	CHECK(tr.peek(9).get_type() == TokenType::END_OF_FILE);
	CHECK(tr.end_of_file_reached() == false);
	CHECK(tr.consume().get_type() == TokenType::PACKAGE);
	CHECK(tr.consume(1).get_type() == TokenType::END_STATEMENT);
	CHECK(tr.end_of_file_reached() == false);
	CHECK(tr.consume().get_type() == TokenType::IDENTIFIER);
	CHECK(tr.end_of_file_reached() == true);
	CHECK(tr.consume().get_type() == TokenType::END_OF_FILE);
	CHECK(tr.consume().get_type() == TokenType::END_OF_FILE);
	CHECK(tr.end_of_file_reached() == true);
}
