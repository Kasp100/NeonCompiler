#include "compiler.hpp"

#include <iostream>
#include <span>
#include "../reading/char_reader.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenisation_error.hpp"
#include "parser/parser.hpp"
#include "parser/parsing_error.hpp"

using namespace neon_compiler;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
	: logger{logger} {}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	std::vector<Token> tokens;
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
			"At line " + std::to_string(error.get_line()) +
			", column " + std::to_string(error.get_column()) +
			", in file \"" + std::string(file_name) +
			"\": " + std::string(error.get_message())
		);
	}

	const std::span<const Token> tokens_view{tokens};
	parser::Parser parser{tokens_view, logger};
	parser.run();

	const std::vector<parser::ParsingError> parser_errors{parser.take_errors()};
	for(const parser::ParsingError& error : parser_errors)
	{
		const Token& token = error.get_token();
		logger->error
		(
			"At line " + std::to_string(token.get_line()) +
			", column " + std::to_string(token.get_column()) +
			" (length: " + std::to_string(token.get_length()) +
			"), in file \"" + std::string(file_name) +
			"\": " + std::string(error.get_message())
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
}
