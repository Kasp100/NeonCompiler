#include "parser.hpp"

using namespace neon_compiler::parser;

Parser::Parser(std::span<neon_compiler::Token> tokens)
	: tokens{tokens} {}
