#ifndef CONSOLE_ANALYSIS_REPORTER_HPP
#define CONSOLE_ANALYSIS_REPORTER_HPP

#include <iostream>
#include "../analysis_reporter.hpp"

namespace neon_compiler::analysis::impl
{

class ConsoleAnalysisReporter : public neon_compiler::analysis::AnalysisReporter
{
public:
	explicit ConsoleAnalysisReporter(const std::string& file, std::ostream& out = std::cout);
	void report(const AnalysisEntry& entry) override;
private:
	std::string file;
	std::ostream& out;
	std::string escape(const std::string& str) const;
	std::string analysis_severity_to_string(const AnalyisSeverity severity) const;
	std::string analysis_entry_type_to_string(const AnalysisEntryType entry_type) const;
};

}

#endif // CONSOLE_ANALYSIS_REPORTER_HPP