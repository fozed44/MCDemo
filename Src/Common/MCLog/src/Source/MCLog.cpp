#include "../Headers/MCLog.h"
#include "../Headers/spdlog/sinks/stdout_color_sinks.h"
#include <iostream>

namespace MC {

	std::shared_ptr<spdlog::logger> MCLog::s_pCoreLogger;
	std::shared_ptr<spdlog::logger> MCLog::s_pXMLLogger;

	MCLog::MCLog() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_pCoreLogger = spdlog::stdout_color_mt("CORE");
		s_pCoreLogger->set_level(spdlog::level::trace);

		s_pXMLLogger = spdlog::stdout_color_mt("XML");
		s_pXMLLogger->set_level(spdlog::level::trace);
	}


	MCLog::~MCLog() {
	}


}