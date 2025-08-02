#ifndef ANALYSIS_REPORTER_HPP
#define ANALYSIS_REPORTER_HPP

#include "analysis_entry.hpp"

namespace neon_compiler::analysis
{

class AnalysisReporter
{
public:
	virtual ~AnalysisReporter() = default;

	virtual void report(const AnalysisEntry& entry) = 0;
};

}

#endif // ANALYSIS_REPORTER_HPP