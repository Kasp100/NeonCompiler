#include <memory>
#include <vector>

#include "logging/logger.hpp"
#include "file_reading/file_reader.hpp"

using namespace std;
using namespace logging;
using namespace file_reading;

int main(int argc, char** argv)
{
    shared_ptr<Logger> logger = make_shared<Logger>();

    if (argc < 2)
    {
        logger->error("Usage: " + string(argv[0]) + " <source file(s)>\n");
        return 1;
    }

    unique_ptr<FileReader> file_reader = make_unique<FileReader>(logger);

    for (int i = 1; i < argc; ++i)
    {
        const char* file_name = argv[i];

        if (!file_reader->read_file(file_name))
        {
            logger->error("Error reading file: " + string(file_name) + "\n");
            return 1;
        }
    }

    // Continue with the next steps of the compiler (parsing, lexing, etc.)
    return 0;
}
