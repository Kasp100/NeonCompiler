#include "logger.hpp"

#include <iostream>
#include <string>

using namespace logging;

Logger::Logger() {}

constexpr const char* PREFIX_LOG = "[Log] ";

void Logger::error(const std::string& message) const
{
	std::cerr << PREFIX_LOG << "\x1B[31mError\033[0m " << message << std::endl;
}

void Logger::warning(const std::string& message) const
{
	std::cerr << PREFIX_LOG << "\x1B[33mWarning\033[0m " << message << std::endl;
}

void Logger::info(const std::string& message) const
{
	std::cerr << PREFIX_LOG << "\x1B[32mInfo\033[0m " << message << std::endl;
}

void Logger::debug(const std::string& message) const
{
	std::cerr << PREFIX_LOG << "\x1B[35mDebug\033[0m " << message << std::endl;
}