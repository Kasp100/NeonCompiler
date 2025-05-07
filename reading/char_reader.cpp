#include "char_reader.hpp"

using namespace reading;

CharReader::CharReader(std::unique_ptr<std::istream> input_stream)
    : reader(std::move(input_stream)) {}

char CharReader::read_next()
{
    if (eof_reached) return END_OF_FILE;

    int val = reader->get();
    if (val == EOF)
    {
        eof_reached = true;
        return END_OF_FILE;
    }

    char c = static_cast<char>(val);
    if (c == '\r')
    {
        char next = static_cast<char>(reader->peek());
        if (next == '\n')
        {
            reader->get(); // consume the '\n'
        }
        return '\n';
    }

    return c;
}

char CharReader::peek(int offset)
{
    try
    {
        while (offset >= static_cast<int>(buffer.size()))
        {
            buffer.push_back(read_next());
        }
        return buffer[offset];
    } catch (...)
    {
        throw ReadException("Error while peeking input stream");
    }
}

char CharReader::consume(int offset)
{
    char result = peek(offset);
    for (int i = 0; i <= offset; ++i)
    {
        consume_next();
    }
    return result;
}

char CharReader::consume_next()
{
    char c = buffer.front();
    buffer.erase(buffer.begin());
    return c;
}

bool CharReader::end_of_file_reached()
{
    return peek() == END_OF_FILE;
}

bool CharReader::consume_if_matches(char match)
{
    if (peek() == match)
    {
        consume();
        return true;
    }
    return false;
}

bool CharReader::consume_all_if_next(const std::string& str)
{
    peek(static_cast<int>(str.size()) - 1); // Ensure enough characters in buffer

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (peek(static_cast<int>(i)) != str[i])
        {
            return false;
        }
    }

    consume(static_cast<int>(str.size()) - 1);
    return true;
}
