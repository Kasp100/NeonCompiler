#include "console_analysis_reporter.hpp"

using namespace neon_compiler::analysis::impl;

ConsoleAnalysisReporter::ConsoleAnalysisReporter(const std::string& file, std::ostream& out)
		: file{file}, out(out)
{
	out << "[AH] file, offset_in_file, newlines_count, offset_in_line, length\n";
}
	
void ConsoleAnalysisReporter::report(const AnalysisEntry& entry)
{
	out << "[A] "
		<< entry.file
		<< " "
		<< analysis_severity_to_string(entry.severity)
		<< " "
		<< analysis_entry_type_to_string(entry.type)
		<< " "
		<< std::to_string(entry.source_position.offset_in_file)
		<< " "
		<< std::to_string(entry.source_position.newlines_count)
		<< " "
		<< std::to_string(entry.source_position.offset_in_line)
		<< " "
		<< std::to_string(entry.length)
		<< " ";
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

std::string ConsoleAnalysisReporter::analysis_severity_to_string(const AnalysisSeverity severity) const
{
	switch (severity)
	{
		case AnalysisSeverity::ERROR: return "ERROR";
		case AnalysisSeverity::WARNING: return "WARNING";
		case AnalysisSeverity::INFO: return "INFO";
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