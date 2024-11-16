#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <string>

class Compiler
{
public:
    void run(const std::string& source_file, const std::string& output_file) const;
private:
    
};

#endif // COMPILER_HPP