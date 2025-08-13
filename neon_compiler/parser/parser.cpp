#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;

Parser::Parser(std::span<const Token> tokens, std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter)
	: reader{tokens}, analysis_reporter{analysis_reporter} {}

void Parser::run()
{
	if(reader.peek().get_type() == TokenType::PACKAGE)
	{
		report_token(AnalysisEntryType::KEYWORD, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::ERROR, reader.peek(), std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	std::optional<Identifier> package_id = parse_identifier(AnalysisEntryType::PACKAGE);
	if(!package_id.has_value())
	{
		report_token(AnalysisEntryType::ERROR, reader.peek(), std::string{error_messages::MISSING_IDENTIFIER});
	}
}

void Parser::report_token(AnalysisEntryType type, const Token& token, std::optional<std::string> info)
{
	analysis_reporter->report(AnalysisEntry{type, token.get_source_position(), token.get_length(), info});
}

std::optional<Identifier> Parser::parse_identifier(AnalysisEntryType type)
{
	std::vector<std::string> parts{};
	std::vector<Token> tokens{};

	bool continue_reading = true;
	do
	{
		if(reader.peek().get_type() != TokenType::IDENTIFIER)
		{
			return std::nullopt;
		}

		Token token = reader.consume();
		tokens.push_back(token);
		parts.emplace_back(token.get_lexeme().value());

		continue_reading = reader.peek().get_type() == TokenType::STATIC_ACCESSOR;
		if(continue_reading)
		{
			tokens.push_back(reader.consume());
		}
	}
	while(continue_reading);

	Identifier id{parts};
	std::string id_string = id.to_string();

	for(const Token& token : tokens)
	{
		report_token(type, token, id_string);
	}

	return id;
}