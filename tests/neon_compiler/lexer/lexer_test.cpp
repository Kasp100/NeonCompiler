#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libs/doctest/doctest.hpp"

#include <memory>
#include <span>
#include "../../../neon_compiler/token.hpp"
#include "../../../neon_compiler/lexer/lexer.hpp"
#include "../../../reading/char_reader.hpp"

using namespace neon_compiler::lexer;

constexpr const char* TEST_KEYWORDS = "pkg interface mut:";

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