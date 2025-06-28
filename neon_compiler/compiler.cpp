#include "compiler.hpp"

#include <iostream>
#include "../reading/char_reader.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenisation_error.hpp"

using namespace neon_compiler;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
	: logger(logger) {}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	std::unique_ptr<reading::CharReader> reader = std::make_unique<reading::CharReader>(move(stream));
	lexer::Lexer lexer(move(reader));

	try
	{
		lexer.run();
	}
	catch (const reading::ReadException& e)
	{
		logger->error("Reading failed: " + std::string(e.what()));
		return;
	}

	for(const lexer::TokenisationError& error : lexer.get_errors())
	{
		logger->error
		(
			"At line " + std::to_string(error.get_line()) +
			", column " + std::to_string(error.get_column()) +
			", in file \"" + std::string(file_name) +
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
