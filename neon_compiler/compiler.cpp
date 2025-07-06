#include "compiler.hpp"

#include <iostream>
#include <span>
#include "../reading/char_reader.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokenisation_error.hpp"

using namespace neon_compiler;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
	: logger{logger} {}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	std::vector<Token> tokens;
	std::vector<lexer::TokenisationError> errors;
	try
	{
		std::unique_ptr<reading::CharReader> reader =
				std::make_unique<reading::CharReader>(std::move(stream));
		lexer::Lexer lexer(std::move(reader));

		lexer.run();

		tokens = lexer.take_tokens();
		errors = lexer.take_errors();
	}
	catch (const reading::ReadException& e)
	{
		logger->error("Reading failed: " + std::string(e.what()));
		return;
	}

	for(const Token& token : tokens)
	{
		logger->debug("Token\t" + std::to_string(static_cast<int>(token.get_type())));
		if(token.get_lexeme().has_value())
		{
			logger->debug("Lexeme =\t" + std::string(token.get_lexeme().value()));
		}
	}

	for(const lexer::TokenisationError& error : errors)
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
