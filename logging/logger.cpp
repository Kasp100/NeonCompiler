#include "logger.hpp"

#include <iostream>
#include <string>

using namespace logging;
using namespace std;

Logger::Logger() {}

void Logger::error(const string& message) const
{
    cerr << "\x1B[31mError\033[0m\t" << message << endl;
}

void Logger::warning(const string& message) const
{
    cerr << "\x1B[33mWarning\033[0m\t" << message << endl;
}

void Logger::info(const string& message) const
{
    cerr << "\x1B[32mInfo\033[0m\t" << message << endl;
}

void Logger::debug(const string& message) const
{
    cerr << "\x1B[35mDebug\033[0m\t" << message << endl;
}