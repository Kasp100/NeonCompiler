#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

namespace logging
{

class Logger
{
public:
    Logger();

    void error(const std::string& message) const;
    void warning(const std::string& message) const;
    void info(const std::string& message) const;
    void debug(const std::string& message) const;
};

}

#endif // LOGGER_HPP