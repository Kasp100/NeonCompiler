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
    PACKAGE
};

struct AnalysisEntry
{
	const AnalysisEntryType type;
    const reading::SourcePosition source_position;
	const uint32_t length;
	const std::optional<std::string> info;
};

}

#endif // ANALYSIS_ENTRY_HPP