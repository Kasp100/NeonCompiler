#include "token_reader.hpp"

using namespace neon_compiler;

TokenReader::TokenReader(std::span<const neon_compiler::Token> tokens)
	: tokens{tokens} {}

const Token& TokenReader::consume(uint offset)
{
	reading_index += offset;
	const Token& token = peek();
	++reading_index;
	return token;
}

const Token& TokenReader::peek(uint offset) const
{
	uint peek_reading_index = reading_index + offset;

	if(peek_reading_index >= tokens.size())
	{
		peek_reading_index = tokens.size() - 1;
	}

	return tokens[peek_reading_index];
}

bool TokenReader::end_of_file_reached() const
{
	return peek().get_type() == TokenType::END_OF_FILE;
}

bool TokenReader::consume_if_matches(const TokenType& match)
{
	bool matching = peek().get_type() == match;
	if(matching)
	{
		consume();
	}
	return matching;
}
