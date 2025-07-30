#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;

Parser::Parser(std::span<const Token> tokens, std::shared_ptr<logging::Logger> logger)
	: reader{tokens}, errors{}, logger{logger} {}

void Parser::run()
{
	if(!reader.consume_if_matches(TokenType::PACKAGE))
	{
		errors.emplace_back(reader.peek(), error_messages::MISSING_PACKAGE_DECLARATION);
	}

	std::optional<Identifier> package_id = parse_identifier();
	if(!package_id.has_value())
	{
		errors.emplace_back(reader.peek(), error_messages::MISSING_IDENTIFIER);
	}
}

std::vector<parser::ParsingError> Parser::take_errors()
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

std::optional<Identifier> Parser::parse_identifier()
{
	std::vector<std::string> parts{};

	do
	{
		if(reader.peek().get_type() != TokenType::IDENTIFIER)
		{
			return std::nullopt;
		}
		parts.emplace_back(reader.consume().get_lexeme().value());
	}
	while(reader.consume_if_matches(TokenType::STATIC_ACCESSOR));

	return Identifier{parts};
}