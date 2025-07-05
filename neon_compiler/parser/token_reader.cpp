#include "token_reader.hpp"

using namespace neon_compiler::parser;

TokenReader::TokenReader(std::span<const neon_compiler::Token> tokens)
	: tokens{tokens} {}
