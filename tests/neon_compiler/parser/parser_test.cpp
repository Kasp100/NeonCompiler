#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../../libs/doctest/doctest.hpp"

#include <vector>
#include "../../../neon_compiler/token.hpp"

using namespace neon_compiler;

TEST_CASE("Fyunctions are parsed correctly")
{
	// Arrange
	std::vector<Token> tokens
	{
		Token{TokenType::PACKAGE, reading::SourcePosition{0,0,0}, 0},
		Token{TokenType::IDENTIFIER, reading::SourcePosition{0,0,0}, 0, ""},
		Token{TokenType::RETURN_TYPE_VOID, reading::SourcePosition{0,0,0}, 0},
		Token{TokenType::IDENTIFIER, reading::SourcePosition{0,0,0}, 0, "main"}
	};

	// Act

	// Assert
}