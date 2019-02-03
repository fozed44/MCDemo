#include "../Headers/MCLog.h"
#include "../Headers/spdlog/sinks/stdout_color_sinks.h"
#include "../Headers/spdlog/sinks/basic_file_sink.h"
#include "libloaderapi.h"

namespace MC {

	MCLog::MCLog() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		_pCoreLogger = spdlog::stdout_color_mt("CORE");
		_pCoreLogger->set_level(spdlog::level::trace);

		_pXMLLogger = spdlog::stdout_color_mt("XML");		
		_pXMLLogger->set_level(spdlog::level::trace);

		try {
			auto initLoggerConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto initLoggerFileSink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetInitLogFilename().c_str(), true);
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

	std::string MCLog::GetExecutingDirectory() const {
		std::string path(MAX_PATH, '\0');

		GetModuleFileNameA(NULL, &path[0], (DWORD)path.length());
		path.resize(strlen(&path[0]));		

		std::string::size_type pos = path.find_last_of("\\/");

		return path.substr(0, pos);
	}

	std::string MCLog::GetInitLogFilename() const {
		auto initLogFilename = GetExecutingDirectory() + __LOG_SUB_DIR__ + __INIT_LOG_FILE_NAME__;
		_pCoreLogger->trace("Using init log: {0}", initLogFilename.c_str());
		return initLogFilename;
	}
}