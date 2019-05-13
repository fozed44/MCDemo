#pragma once

#include <string>

namespace MC {

	/*	MCIAnalyzer
		
		An analyzer is a class that performs non-intrusive analysis of another object or objects, 
		returning the analysis as a formatted string. */
	class MCIAnalyzer {
	public:
		virtual ~MCIAnalyzer() {}
		virtual std::string Analyze() = 0;
	};
}
