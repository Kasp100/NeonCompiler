#include "compiler.hpp"

#include <iostream>
#include <span>
#include "../reading/char_reader.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenisation_error.hpp"
#include "analysis/analysis_reporter.hpp"
#include "analysis/impl/console_analysis_reporter.hpp"
#include "ast/ast_visitor.hpp"
#include "ast/impl/ast_printer.hpp"

using namespace logging;
using namespace reading;
using namespace neon_compiler;
using namespace neon_compiler::analysis;
using namespace neon_compiler::analysis::impl;
using namespace neon_compiler::ast::impl;
using namespace neon_compiler::ast::nodes;
using namespace neon_compiler::lexer;
using namespace neon_compiler::parser;

Compiler::Compiler(std::shared_ptr<Logger> init_logger)
	: logger{init_logger}
{
	root_node = std::make_shared<Root>();
	operator_map = std::make_shared<OperatorMap>();
}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	std::vector<lexer::TokenisationError> lexer_errors;
	try
	{
		std::unique_ptr<CharReader> reader = std::make_unique<CharReader>(std::move(stream));
		Lexer lexer(std::move(reader));

		lexer.run();

		file_tokens.emplace(std::string{file_name}, lexer.take_tokens());
		lexer_errors = lexer.take_errors();
	}
	catch (const ReadException& e)
	{
		logger->error("Reading failed: " + std::string(e.what()));
		return;
	}

	for(const TokenisationError& error : lexer_errors)
	{
		logger->error
		(
			"At line " + std::to_string(error.source_position.newlines_count + 1) +
			", column " + std::to_string(error.source_position.offset_in_line + 1) + // TODO: Take into account '\t'
			", in file \"" + std::string(file_name) +
			"\": " + std::string(error.message)
		);
	}
}

void Compiler::build() const
{
	logger->debug("Building...");
}

void Compiler::generate_analysis() const
{
	logger->debug("Generating analysis...");

	std::vector<Parser> parsers;

	for(const std::pair<const std::string, std::vector<Token>>& pair : file_tokens)
	{
		const std::span<const Token> tokens_view{pair.second};

		std::shared_ptr<AnalysisReporter> reporter = std::make_shared<ConsoleAnalysisReporter>(pair.first);

		parsers.emplace_back(logger, tokens_view, reporter, root_node, pair.first, operator_map);
	}

	for(Parser& parser : parsers)
	{
		parser.run_a();
	}

	std::shared_ptr<OperatorTable> operator_table = std::make_shared<OperatorTable>();

	for(Parser& parser : parsers)
	{
		parser.run_b(operator_table);
	}

	ASTPrinter printer{};
	printer.visit(*root_node);
}
