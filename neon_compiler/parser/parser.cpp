#include "parser.hpp"

using namespace neon_compiler::parser;

Parser::Parser(std::span<const neon_compiler::Token> tokens, std::shared_ptr<logging::Logger> logger)
	: reader{tokens}, errors{}, logger{logger} {}

void Parser::run()
{
	if(!reader.consume_if_matches(neon_compiler::TokenType::PACKAGE))
	{
		errors.emplace_back(reader.peek(), error_messages::MISSING_PACKAGE_DECLARATION);
	}
}

std::vector<neon_compiler::parser::ParsingError> Parser::take_errors()
{
	return std::move(errors);
}

void Parser::print_token(const Token& token)
{
	logger->debug("Token\t" + std::to_string(static_cast<int>(token.get_type())));
	if(token.get_lexeme().has_value())
	{
		logger->debug("Lexeme =\t" + std::string(token.get_lexeme().value()));
	}
}