#ifndef ANALYSIS_ENTRY_HPP
#define ANALYSIS_ENTRY_HPP

#include <string_view>
#include "../token.hpp"

namespace neon_compiler::analysis
{

enum class AnalysisEntryType
{
    ERROR,
    WARNING,
    INFO,
    KEYWORD,
    SYMBOL,
    LITERAL_NUMBER,
    LITERAL_CHAR,
    LITERAL_STRING,
    DECLARATION,
    REFERENCE,
};

struct AnalysisEntry
{
	AnalysisEntryType type;
	uint32_t line, column, length;
	std::optional<std::string> info;
};

}

#endif // ANALYSIS_ENTRY_HPP