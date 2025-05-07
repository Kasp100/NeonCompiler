#include "compiler.hpp"

#include <iostream>
#include "../reading/char_reader.hpp"

using namespace neon_compiler;
using namespace std;

Compiler::Compiler(std::shared_ptr<logging::Logger> logger)
    : logger(logger) {}

void Compiler::read_file(std::unique_ptr<std::istream> stream)
{
    logger->debug("Starting tokenisation...");

    reading::CharReader reader(move(stream));

    try
    {
        while (!reader.end_of_file_reached())
        {
            char c = reader.consume();
            logger->debug("Read character: " + std::string(1, c));
            // Tokenise here
        }
    }
    catch (const reading::ReadException& e)
    {
        logger->error("Tokenisation failed: " + std::string(e.what()));
    }
}

void Compiler::build() {
    logger->debug("Building... (codegen steps go here)");
}
