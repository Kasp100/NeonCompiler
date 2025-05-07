#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <string>
#include <memory>
#include <fstream>
#include "../logging/logger.hpp"

namespace file_reading
{

class FileReader
{
public:
    FileReader(std::shared_ptr<logging::Logger> init_logger);

    bool open_file(const char* file_name);
    std::unique_ptr<std::istream> move_stream();

private:
    std::shared_ptr<logging::Logger> logger;
    std::unique_ptr<std::ifstream> input_stream;
};

}

#endif // FILE_READER_HPP
