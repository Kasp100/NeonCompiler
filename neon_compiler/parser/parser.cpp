#include "parser.hpp"

using namespace neon_compiler::parser;

Parser::Parser(std::span<const neon_compiler::Token> tokens, std::shared_ptr<logging::Logger> logger)
	: reader{tokens}, logger{logger} {}

void Parser::run()
{
	print_token(reader.consume());
}

void Parser::print_token(const Token& token)
{
	logger->debug("Token\t" + std::to_string(static_cast<int>(token.get_type())));
	if(token.get_lexeme().has_value())
	{
		logger->debug("Lexeme =\t" + std::string(token.get_lexeme().value()));
	}
}