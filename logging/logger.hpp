#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

namespace logging
{

class Logger
{
public:
    Logger();

    void error(const std::string& message);
    void warning(const std::string& message);
    void info(const std::string& message);
    void debug(const std::string& message);
};

}

#endif // LOGGER_HPP