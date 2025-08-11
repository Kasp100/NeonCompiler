#ifndef CHAR_READER_HPP
#define CHAR_READER_HPP

#include <cstdint>
#include <deque>
#include <istream>
#include <optional>
#include <memory>
#include <stdexcept>
#include "char_w_source_position.hpp"

namespace reading
{
	class ReadException : public std::runtime_error
	{
	public:
		explicit ReadException(const std::string& msg)
			: std::runtime_error{msg} {}
	};

	/** A peek/consume reader for characters (`unsigned char`) from an `std::istream`.
	 * It normalises newlines (`\r` and `\r\n` become `\n`), and keeps track of source positions. */
	class CharReader
	{
	public:
		explicit CharReader(std::unique_ptr<std::istream> input_stream);

		unsigned char consume(uint offset = 0);
		unsigned char peek(uint offset = 0);
		bool end_of_file_reached();
		SourcePosition get_source_position() const;
		bool consume_if_matches(unsigned char match);
		bool consume_all_if_next(const std::string& str);

	private:
		std::unique_ptr<std::istream> reader;
		std::deque<CharWSourcePosition> buffer;
		bool eof_reached = false;

		// stream source position
		uint32_t offset_in_file = 0;
		uint32_t newlines_count = 0;
		uint32_t offset_in_line = 0;

		std::optional<CharWSourcePosition> read_next();
		int read_next_byte();
		std::optional<unsigned char> convert(int val) const;
		void consume_from_buffer();
	};

}

#endif // CHAR_READER_HPP