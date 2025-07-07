#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../../libs/doctest/doctest.hpp"

#include <memory>
#include <vector>
#include "../../../../neon_compiler/parser/token_reader.hpp"

using namespace neon_compiler::parser;

static const std::vector<neon_compiler::Token> TEST_TOKENS
{
	neon_compiler::Token{neon_compiler::TokenType::PACKAGE,0,0,0,std::nullopt},
	neon_compiler::Token{neon_compiler::TokenType::IDENTIFIER,0,0,0,"main"},
	neon_compiler::Token{neon_compiler::TokenType::END_STATEMENT,0,0,0,std::nullopt}
};

TEST_CASE("Token reader works")
{
	// Arrange
	std::span<const neon_compiler::Token> tokens = std::span{TEST_TOKENS};
	TokenReader tr{tokens};

	// Act & Assert
	CHECK(tr.peek().get_type() == neon_compiler::TokenType::PACKAGE);
	CHECK(tr.peek(1).get_type() == neon_compiler::TokenType::IDENTIFIER);
	CHECK(tr.peek(1).get_lexeme().value() == "main");
	CHECK(tr.peek(2).get_type() == neon_compiler::TokenType::END_STATEMENT);
	CHECK(tr.peek(100).get_type() == neon_compiler::TokenType::END_OF_FILE);
	CHECK(tr.consume().get_type() == neon_compiler::TokenType::PACKAGE);
	CHECK(tr.consume(1).get_type() == neon_compiler::TokenType::END_STATEMENT);
	CHECK(tr.consume().get_type() == neon_compiler::TokenType::END_OF_FILE);
}
