#include "logger.hpp"

#include <iostream>
#include <string>

using namespace logging;
using namespace std;

Logger::Logger()
{
}

void Logger::error(const string& message)
{
    cerr << "Error: " << message << endl;
}

void Logger::warning(const string& message)
{
    cerr << "Warning: " << message << endl;
}

void Logger::info(const string& message)
{
    cerr << "Info: " << message << endl;
}

void Logger::debug(const string& message)
{
    cerr << "Debug: " << message << endl;
}