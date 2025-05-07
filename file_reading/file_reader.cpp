#include "file_reader.hpp"

#include <fstream>

using namespace file_reading;
using namespace std;

FileReader::FileReader(shared_ptr<logging::Logger> init_logger)
{
    file_content = make_unique<std::string>();
    logger = init_logger;
}

bool FileReader::read_file(const char* file_name)
{
    ifstream file(file_name);

    if (!file)
    {
        logger->error("Failed to open file: " + string(file_name));
        return false;
    }

    // Read the file content into the string, managed by unique_ptr
    *file_content = std::string((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

    // Optionally print the file content to verify
    logger->debug("File content of " + string(file_name) + ": \n" + string(*file_content) + "\n");

    return true;
}
