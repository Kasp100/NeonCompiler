#include "token_reader.hpp"

using namespace neon_compiler;

static const neon_compiler::Token END_OF_FILE_TOKEN{neon_compiler::TokenType::END_OF_FILE,0,0,0,std::nullopt};

TokenReader::TokenReader(std::span<const neon_compiler::Token> tokens)
	: tokens{tokens} {}

const neon_compiler::Token& TokenReader::consume(uint offset)
{
	reading_index += offset;
	const neon_compiler::Token& token = peek();
	++reading_index;
	return token;
}

const neon_compiler::Token& TokenReader::peek(uint offset) const
{
	uint peek_reading_index = reading_index + offset;

	if(peek_reading_index >= tokens.size()) { return END_OF_FILE_TOKEN; }

	return tokens[peek_reading_index];
}

bool TokenReader::end_of_file_reached() const
{
	return reading_index >= tokens.size();
}

bool TokenReader::consume_if_matches(const neon_compiler::TokenType& match)
{
	bool matching = peek().get_type() == match;
	if(matching)
	{
		consume();
	}
	return matching;
}
