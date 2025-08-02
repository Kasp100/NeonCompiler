#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;

Parser::Parser(std::span<const Token> tokens, std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter)
	: reader{tokens}, analysis_reporter{analysis_reporter} {}

void Parser::run()
{
	if(!reader.consume_if_matches(TokenType::PACKAGE))
	{
		report_token(AnalysisEntryType::ERROR, reader.peek(), std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	std::optional<Identifier> package_id = parse_identifier();
	if(!package_id.has_value())
	{
		report_token(AnalysisEntryType::ERROR, reader.peek(), std::string{error_messages::MISSING_IDENTIFIER});
	}
}

void Parser::report_token(AnalysisEntryType type, const Token& token, std::optional<std::string> info = std::nullopt)
{
	analysis_reporter->report(AnalysisEntry{type, token.get_line(), token.get_column(), token.get_length(), info});
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