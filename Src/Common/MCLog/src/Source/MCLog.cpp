#include "../Headers/MCLog.h"
#include <iostream>

namespace MC {

	MCLog::MCLog()
	{
	}


	MCLog::~MCLog()
	{
	}

	void MCLog::Log(const char* const logger, MC_LOG_LEVEL level, const char* const msg) const {
		std::cout << logger << ": " << msg << std::endl;
	}

}