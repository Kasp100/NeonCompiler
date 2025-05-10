#include "tokeniser.hpp"

using namespace compiler;
using namespace std;

Tokeniser::Tokeniser(shared_ptr<logging::Logger> logger, unique_ptr<reading::CharReader> reader)
    : logger(logger), reader(move(reader)) {}

void Tokeniser::run()
{
    while (!reader->end_of_file_reached())
    {
    }
}