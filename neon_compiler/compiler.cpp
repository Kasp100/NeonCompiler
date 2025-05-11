#include "compiler.hpp"

#include <iostream>
#include "../reading/char_reader.hpp"
#include "tokeniser.hpp"

using namespace neon_compiler;
using namespace std;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
	: logger(logger) {}

void Compiler::read_file(std::unique_ptr<std::istream> stream, std::string_view file_name)
{
	unique_ptr<reading::CharReader> reader = make_unique<reading::CharReader>(move(stream));
	compiler::Tokeniser tokeniser(logger, move(reader));

	try
	{
		tokeniser.run();
	}
	catch (const reading::ReadException& e)
	{
		logger->error("Reading failed: " + std::string(e.what()));
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
