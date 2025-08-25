#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;
using namespace neon_compiler::ast::nodes;

Parser::Parser
(
	std::span<const Token> tokens,
	std::shared_ptr<neon_compiler::analysis::AnalysisReporter> analysis_reporter,
	std::shared_ptr<neon_compiler::ast::nodes::Root> root_node,
	std::string_view file
) :
	reader{tokens}, analysis_reporter{analysis_reporter}, root_node{root_node}, file{file} {}

void Parser::run()
{
	if(reader.peek().get_type() == TokenType::PACKAGE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalyisSeverity::ERROR, reader.peek(), std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	std::optional<Identifier> package_id = parse_identifier(AnalysisEntryType::PACKAGE, AnalyisSeverity::INFO);
	if(!package_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalyisSeverity::ERROR, reader.peek(), std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	while(!reader.end_of_file_reached())
	{
		Access access = parse_access();
		parse_package_member(access);
	}
}

void Parser::report_token(AnalysisEntryType type, AnalyisSeverity severity, const Token& token, std::optional<std::string> info)
{
	analysis_reporter->report(AnalysisEntry{type, severity, token.get_source_position(), token.get_length(), info});
}

std::optional<Identifier> Parser::parse_identifier(AnalysisEntryType type, AnalyisSeverity severity)
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
		report_token(type, severity, token, id_string);
	}

	return id;
}

Access Parser::parse_access()
{
	Access access{AccessType::PRIVATE};
	if(reader.peek().get_type() == TokenType::ACCESS_PRIVATE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PROTECTED)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::ERROR, reader.consume(), std::string{error_messages::PROTECTED_PACKAGE_MEMBER});
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PUBLIC)
	{
		access = Access{AccessType::PUBLIC};
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_EXCLUSIVE)
	{
		access = Access{AccessType::EXCLUSIVE}; // TODO: parse package member pattern (see spec)
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	return access;
}

void Parser::parse_package_member(const Access& access)
{
	if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_ENTRYPOINT)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_PURE_FUNCTION_SET)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_COMPILE_FUNCTION)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::MEMBER_ABSTRACT && reader.peek(1).get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_INTERFACE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::KEYWORD, AnalyisSeverity::ERROR, reader.consume(), std::string{error_messages::INVALID_FILE_LEVEL_TOKEN});
	}
}