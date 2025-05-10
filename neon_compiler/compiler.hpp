#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include "../logging/logger.hpp"

namespace neon_compiler
{

class Compiler
{
public:
    explicit Compiler(std::shared_ptr<logging::Logger> logger);

    void read_file(std::unique_ptr<std::istream> stream);
    void build();

private:
    std::shared_ptr<logging::Logger> logger;
};

}

#endif // COMPILER_HPP