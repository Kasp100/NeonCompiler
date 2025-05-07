#include "file_reader.hpp"

#include <cstring>
#include <cerrno>

using namespace file_reading;
using namespace std;

FileReader::FileReader(shared_ptr<logging::Logger> init_logger)
{
    input_stream = nullptr;
    logger = init_logger;
}

bool FileReader::open_file(const char* file_name)
{
    input_stream = std::make_unique<std::ifstream>(file_name);

    if (!input_stream->is_open())
    {
        logger->error("Failed to open file: " + string(file_name) +
                      " - " + std::strerror(errno));
        return false;
    }

    return true;
}

std::unique_ptr<std::istream> FileReader::move_stream()
{
    if (!input_stream)
    {
        logger->error("Attempted to move an unopened stream.");
    }

    return std::move(input_stream);
}
