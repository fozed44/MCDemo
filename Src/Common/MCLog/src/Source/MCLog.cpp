#include "../Headers/MCLog.h"
#include "../Headers/spdlog/sinks/stdout_color_sinks.h"
#include "../Headers/spdlog/sinks/basic_file_sink.h"
#include "libloaderapi.h"
#include "../../../MCCommon/src/Headers/Paths.h"

namespace MC {

	MCLog::MCLog() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		_pCoreLogger = spdlog::stdout_color_mt("CORE");
		_pCoreLogger->set_level(spdlog::level::trace);

		_pXMLLogger = spdlog::stdout_color_mt("XML");		
		_pXMLLogger->set_level(spdlog::level::trace);

		try {
			auto initLoggerConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto initLoggerFileSink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(Paths::InitLogFilename(), true);
			initLoggerConsoleSink->set_level(spdlog::level::trace);
			initLoggerConsoleSink->set_pattern("%^[%T] %n: %v%$");
			initLoggerFileSink->set_level(spdlog::level::trace);
			initLoggerFileSink->set_pattern("[%T] [%l] %v");
			_pInitLogger = std::shared_ptr<spdlog::logger>(new spdlog::logger("Init", { initLoggerConsoleSink, initLoggerFileSink }));
		}
		catch (...) {
			_pCoreLogger->error("Failed to initialize Init logger.");
			throw;
		}
		_pInitLogger->set_level(spdlog::level::trace);
		_pInitLogger->flush_on(spdlog::level::trace);
	}

	MCLog::~MCLog() { }
}