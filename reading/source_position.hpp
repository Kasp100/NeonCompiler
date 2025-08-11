#ifndef SOURCE_POSITION_HPP
#define SOURCE_POSITION_HPP

#include <cstdint>

namespace reading
{

struct SourcePosition
{
	/** 0-based absolute byte offset */
	uint32_t offset_in_file;
	/** number of newline characters (of any kind) from start (0 for first line) */
	uint32_t newlines_count;
	/** 0-based byte offset from start of the line (not visual column; '\t' is not expanded) */
	uint32_t offset_in_line;
};

}

#endif // SOURCE_POSITION_HPP