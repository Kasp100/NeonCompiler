#include "compiler.hpp"

#include <iostream>
#include <span>
#include "../reading/char_reader.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenisation_error.hpp"
#include "parser/parser.hpp"
#include "analysis/analysis_reporter.hpp"
#include "analysis/impl/console_analysis_reporter.hpp"
#include "ast/ast_visitor.hpp"
#include "ast/impl/ast_printer.hpp"

using namespace neon_compiler;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
	: logger{logger}, tokens{}
{
	root_node = std::make_shared<neon_compiler::ast::nodes::Root>();
}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	latest_file = std::string{file_name};

	std::vector<lexer::TokenisationError> lexer_errors;
	try
	{
		std::unique_ptr<reading::CharReader> reader =
				std::make_unique<reading::CharReader>(std::move(stream));
		lexer::Lexer lexer(std::move(reader));

		lexer.run();

		tokens = lexer.take_tokens();
		lexer_errors = lexer.take_errors();
	}
	catch (const reading::ReadException& e)
	{
		logger->error("Reading failed: " + std::string(e.what()));
		return;
	}

	for(const lexer::TokenisationError& error : lexer_errors)
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
	const std::span<const Token> tokens_view{tokens};
}

void Compiler::generate_analysis() const
{
	logger->debug("Generating analysis...");
	const std::span<const Token> tokens_view{tokens};

	std::shared_ptr<analysis::AnalysisReporter> reporter =
			std::make_shared<analysis::impl::ConsoleAnalysisReporter>(latest_file);

	parser::Parser parser{tokens_view, reporter, root_node, latest_file};
	parser.run();

	ast::impl::ASTPrinter printer{};
	printer.visit(*parser.get_root_node());

	//std::unique_ptr<ast::ASTVisitor> ast_visitor = std::make_unique<ast::impl::ASTPrinter>();


}
