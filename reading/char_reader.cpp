#include "char_reader.hpp"

#include "char_w_source_position.hpp"

using namespace reading;

CharReader::CharReader(std::unique_ptr<std::istream> input_stream)
	: reader{std::move(input_stream)} {}

std::optional<CharWSourcePosition> CharReader::read_next()
{
	std::optional<unsigned char> c = convert(read_next_byte());
	if(!c.has_value())
	{
		return std::nullopt;
	}

	// Normalise newlines
	if(c == '\r')
	{
		if(convert(reader->peek()) == '\n')
		{
			read_next_byte();
		}
		c = '\n';
	}

	if(c == '\n')
	{
		++newlines_count;
		offset_in_line = 0;
	}

	SourcePosition sp{offset_in_file, newlines_count, offset_in_line};
	return CharWSourcePosition{c.value(), sp};
}

int CharReader::read_next_byte()
{
	if(eof_reached) { return EOF; }

	int val = reader->get();
	++offset_in_file;
	++offset_in_line;

	return val;
}

std::optional<unsigned char> CharReader::convert(int val) const
{
	if(val == EOF)
	{
		return std::nullopt;
	}

	return static_cast<unsigned char>(val);
}

unsigned char CharReader::peek(uint offset)
{
	try
	{
		while(static_cast<uint>(buffer.size()) <= offset)
		{
			std::optional<reading::CharWSourcePosition> next = read_next();
			if(!next.has_value())
			{
				return ' '; // Return a space character to avoid returning optional
			}
			buffer.push_back(next.value());
		}
		return buffer[offset].c;
	}
	catch(...)
	{
		throw ReadException{"Error while peeking input stream"};
	}
}

unsigned char CharReader::consume(uint offset)
{
	unsigned char result = peek(offset);
	for(uint i = 0; i <= offset; ++i)
	{
		consume_from_buffer();
	}
	return result;
}

void CharReader::consume_from_buffer()
{
	if(!buffer.empty())
	{
		buffer.erase(buffer.begin());
	}
}

bool CharReader::end_of_file_reached()
{
	peek();
	return buffer.empty();
}

SourcePosition CharReader::get_source_position() const
{
	if(!buffer.empty())
	{
		return buffer.front().sp;
	}
	return SourcePosition{offset_in_file, newlines_count, offset_in_line};
}

bool CharReader::consume_if_matches(unsigned char match)
{
	if(peek() == match)
	{
		consume();
		return true;
	}
	return false;
}

bool CharReader::consume_all_if_next(const std::string& str)
{
	peek(static_cast<int>(str.size()) - 1); // Ensure enough characters in buffer for fast reading

	for(size_t i = 0; i < str.size(); ++i)
	{
		if(peek(static_cast<int>(i)) != str[i])
		{
			return false;
		}
	}

	consume(static_cast<int>(str.size()) - 1);
	return true;
}
