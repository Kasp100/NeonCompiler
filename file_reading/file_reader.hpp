#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <string>
#include <memory>
#include "../logging/logger.hpp"

namespace file_reading
{

class FileReader
{
public:
    FileReader(std::shared_ptr<logging::Logger> init_logger);

    bool read_file(const char* file_name);

private:
    std::shared_ptr<logging::Logger> logger;
    std::unique_ptr<std::string> file_content;
};

}

#endif // FILE_READER_HPP
