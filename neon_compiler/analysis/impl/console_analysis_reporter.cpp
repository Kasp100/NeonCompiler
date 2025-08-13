#include "console_analysis_reporter.hpp"

using namespace neon_compiler::analysis::impl;

ConsoleAnalysisReporter::ConsoleAnalysisReporter(const std::string& file, std::ostream& output)
		: file{file}, out(output) {}
	
void ConsoleAnalysisReporter::report(const AnalysisEntry& entry)
{
	out << "[A] "
		<< analysis_type_to_string(entry.type)
		<< " "
		<< file
		<< ", OiF " << std::to_string(entry.source_position.offset_in_file)
		<< ", NlC " << std::to_string(entry.source_position.newlines_count)
		<< ", OiL " << std::to_string(entry.source_position.offset_in_line);
	if(entry.info.has_value())
	{
		out << ": ";
		out << escape(entry.info.value());
	}
	out << "\n";
}

std::string ConsoleAnalysisReporter::escape(const std::string& str) const
{
	std::string o;
	for (char c : str)
	{
		switch (c)
		{
			case '\n': { o += "  "; break; }
			default: { o += c; break; }
		}
	}
	return o;
}

std::string ConsoleAnalysisReporter::analysis_type_to_string(const AnalysisEntryType analysis_type) const
{
	switch (analysis_type)
	{
		case AnalysisEntryType::ERROR: return "ERROR";
		case AnalysisEntryType::WARNING: return "WARNING";
		case AnalysisEntryType::INFO: return "INFO";
		case AnalysisEntryType::KEYWORD: return "KEYWORD";
		case AnalysisEntryType::SYMBOL: return "SYMBOL";
		case AnalysisEntryType::LITERAL_NUMBER: return "LITERAL_NUMBER";
		case AnalysisEntryType::LITERAL_CHAR: return "LITERAL_CHAR";
		case AnalysisEntryType::LITERAL_STRING: return "LITERAL_STRING";
		case AnalysisEntryType::DECLARATION: return "DECLARATION";
		case AnalysisEntryType::REFERENCE: return "REFERENCE";
		case AnalysisEntryType::PACKAGE: return "PACKAGE";
		default: return "UNKNOWN";
	}
}