#include "../Headers/MCLog.h"
#include "../Headers/spdlog/sinks/stdout_color_sinks.h"
#include "../Headers/spdlog/sinks/basic_file_sink.h"
#include "../../../MCCommon/src/Headers/Paths.h"

namespace MC {

	std::shared_ptr<spdlog::logger> MCLog::_pCoreLogger(nullptr);
	std::shared_ptr<spdlog::logger> MCLog::_pInitLogger(nullptr);
	std::shared_ptr<spdlog::logger> MCLog::_pXMLLogger(nullptr);

	MCLog::MCLog() {
		if (!_pCoreLogger)
			throw std::exception("Core logger is not initialized.");
		if (!_pInitLogger)
			throw std::exception("Init logger is not initialized.");
		if (!_pXMLLogger)
			throw std::exception("XML logger is not initialized.");
	}

	MCLog::~MCLog() { }
}