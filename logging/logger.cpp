#include "logger.hpp"

#include <iostream>
#include <string>

using namespace logging;
using namespace std;

Logger::Logger() {}

constexpr const char* PREFIX_LOG = "[Log] ";

void Logger::error(const string& message) const
{
    cerr << PREFIX_LOG << "\x1B[31mError\033[0m " << message << endl;
}

void Logger::warning(const string& message) const
{
    cerr << PREFIX_LOG << "\x1B[33mWarning\033[0m " << message << endl;
}

void Logger::info(const string& message) const
{
    cerr << PREFIX_LOG << "\x1B[32mInfo\033[0m " << message << endl;
}

void Logger::debug(const string& message) const
{
    cerr << PREFIX_LOG << "\x1B[35mDebug\033[0m " << message << endl;
}