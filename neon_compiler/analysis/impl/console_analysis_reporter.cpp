#include "console_analysis_reporter.hpp"

using namespace neon_compiler::analysis::impl;

ConsoleAnalysisReporter::ConsoleAnalysisReporter(const std::string& file, std::ostream& out)
		: file{file}, out(out)
{
	out << "[AH]\t\t\tI\tY\tX\tL\n";
}
	
void ConsoleAnalysisReporter::report(const AnalysisEntry& entry)
{
	out << "[A] "
		<< analysis_severity_to_string(entry.severity)
		<< " "
		<< analysis_entry_type_to_string(entry.type)
		<< "\t"
		<< std::to_string(entry.source_position.offset_in_file)
		<< "\t"
		<< std::to_string(entry.source_position.newlines_count)
		<< "\t"
		<< std::to_string(entry.source_position.offset_in_line)
		<< "\t"
		<< std::to_string(entry.length)
		<< "\t";
	if(entry.info.has_value())
	{
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

std::string ConsoleAnalysisReporter::analysis_severity_to_string(const AnalyisSeverity severity) const
{
	switch (severity)
	{
		case AnalyisSeverity::ERROR: return "ERROR";
		case AnalyisSeverity::WARNING: return "WARNING";
		case AnalyisSeverity::INFO: return "INFO";
		default: return "UNKNOWN";
	}
}

std::string ConsoleAnalysisReporter::analysis_entry_type_to_string(const AnalysisEntryType entry_type) const
{
	switch (entry_type)
	{
		case AnalysisEntryType::KEYWORD: return "KEYWORD";
		case AnalysisEntryType::SEPARATOR: return "SEPARATOR";
		case AnalysisEntryType::OPERATOR: return "OPERATOR";
		case AnalysisEntryType::LITERAL_NUMBER: return "LITERAL_NUMBER";
		case AnalysisEntryType::LITERAL_CHAR: return "LITERAL_CHAR";
		case AnalysisEntryType::LITERAL_STRING: return "LITERAL_STRING";
		case AnalysisEntryType::DECLARATION: return "DECLARATION";
		case AnalysisEntryType::REFERENCE: return "REFERENCE";
		case AnalysisEntryType::PACKAGE: return "PACKAGE";
		default: return "UNKNOWN";
	}
}