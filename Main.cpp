#include "neon_compiler/Compiler.hpp"
#include <iostream>

int main()
{
    Compiler compiler{};

    try
    {
        compiler.run("source_file.txt", "output_file.txt");
        std::cout << "Compilation successful!" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Compilation failed: " << ex.what() << std::endl;
    }

    return 0;
}
