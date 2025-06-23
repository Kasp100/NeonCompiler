#ifndef CHAR_READER_HPP
#define CHAR_READER_HPP

#include <cstdint>
#include <istream>
#include <vector>
#include <memory>
#include <stdexcept>

namespace reading
{
    class ReadException : public std::runtime_error
    {
    public:
        explicit ReadException(const std::string& msg)
            : std::runtime_error(msg) {}
    };

    class CharReader
    {
    public:
        static constexpr char END_OF_FILE = 0;

        explicit CharReader(std::unique_ptr<std::istream> input_stream);

        char consume(int offset = 0);
        char peek(int offset = 0);
        bool end_of_file_reached();
        bool consume_if_matches(char match);
        bool consume_all_if_next(const std::string& str);
        std::uint64_t get_line_number() const;
        std::uint64_t get_column_number() const;

    private:
        std::unique_ptr<std::istream> reader;
        std::vector<char> buffer;
        bool eof_reached = false;
        std::uint64_t line_number = 1;
        std::uint64_t column_number = 1;

        char read_next();
        char consume_next();
    };

}

#endif // CHAR_READER_HPP