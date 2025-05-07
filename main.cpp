#include <memory>
#include <vector>

#include "logging/logger.hpp"
#include "file_reading/file_reader.hpp"
#include "neon_compiler/compiler.hpp"

using namespace std;
using namespace logging;

int main(int argc, char** argv)
{
    shared_ptr<Logger> logger = make_shared<Logger>();

    if (argc < 2)
    {
        logger->error("Usage: " + string(argv[0]) + " <source file(s)>\n");
        return 1;
    }

    neon_compiler::Compiler compiler(logger);

    for (int i = 1; i < argc; ++i)
    {
        const char* file_name = argv[i];

        unique_ptr<file_reading::FileReader> file_reader = make_unique<file_reading::FileReader>(logger);

        if (!file_reader->open_file(file_name))
        {
            return 1;
        }

        compiler.read_file(file_reader->move_stream());
    }

    compiler.build();

    return 0;
}
