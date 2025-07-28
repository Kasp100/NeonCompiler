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

	std::string package_name{};
	while(!reader.consume_if_matches(TokenType::END_STATEMENT))
	{
		if(reader.end_of_file_reached())
		{
			errors.emplace_back(reader.peek(), error_messages::UNEXPECTED_END_OF_FILE);
			return;
		}
		else if(reader.peek().get_type() == TokenType::IDENTIFIER)
		{
			std::string id_part{reader.consume().get_lexeme().value()};
			package_name.append(id_part);
		}
		else if(reader.consume_if_matches(TokenType::STATIC_ACCESSOR))
		{
			package_name.append("::");
		}
		else
		{
			errors.emplace_back(reader.consume(), error_messages::UNEXPECTED_TOKEN_IN_PACKAGE_IDENTIFIER);
		}
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