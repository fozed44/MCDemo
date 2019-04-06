#include "../Headers/MCLog.h"
#include "../Headers/spdlog/sinks/stdout_color_sinks.h"
#include "../Headers/spdlog/sinks/basic_file_sink.h"
#include "../Headers/spdlog/sinks/null_sink.h"
#include "../../../MCCommon/src/Headers/Paths.h"

namespace MC {

	void MCLogHelper::SetInitLogger(std::shared_ptr<spdlog::logger> pLogger) {
		MCLog::_pInitLogger = pLogger;
	}

	void MCLogHelper::SetCoreLogger(std::shared_ptr<spdlog::logger> pLogger) {
		MCLog::_pCoreLogger = pLogger;
	}

	void MCLogHelper::SetXMLLogger(std::shared_ptr<spdlog::logger> pLogger) {
		MCLog::_pXMLLogger = pLogger;
	}

	void MCLogHelper::SetRenderLogger(std::shared_ptr<spdlog::logger> pLogger) {
		MCLog::_pRenderLogger = pLogger;
	}

	const char *MCLogHelper::GetDefaultConsolePattern() {
		return "%^[%T] %n: %v%$";
	}

	const char *MCLogHelper::GetDefaultFilePattern() {
		return "[%T] [%l] %v";
	}

	std::shared_ptr<spdlog::logger> MCLogHelper::CreateStdLoggerMT(const char *name, const char *pattern) {
		std::shared_ptr<spdlog::logger> result = spdlog::stdout_color_mt(name);
		result->set_level(spdlog::level::trace);
		if (pattern)
			result->set_pattern(pattern);
		return result;
	}

	std::shared_ptr<spdlog::logger> MCLogHelper::CreateFileLoggerMT(const char *loggerName, const char *filename, const char *pattern) {
		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
		fileSink->set_level(spdlog::level::trace);

		if (pattern)
			fileSink->set_pattern(pattern);
		else
			fileSink->set_pattern(GetDefaultConsolePattern());

		auto result = std::shared_ptr<spdlog::logger>(new spdlog::logger(loggerName, { fileSink }));
		return result;
	}

	std::shared_ptr<spdlog::logger> MCLogHelper::CreateDualLoggerMT(
		const char *loggerName,
		const char *filename,
		const char *consolePattern,
		const char *filePattern
	) {
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);

		fileSink   ->set_level(spdlog::level::trace);
		consoleSink->set_level(spdlog::level::trace);

		if (consolePattern)
			consoleSink->set_pattern(consolePattern);
		else
			consoleSink->set_pattern(GetDefaultConsolePattern());

		if (filePattern)
			fileSink->set_pattern(filePattern);
		else
			fileSink->set_pattern(GetDefaultFilePattern());

		auto result = std::shared_ptr<spdlog::logger>(new spdlog::logger(loggerName, { consoleSink, fileSink }));
		result->flush_on(spdlog::level::trace);
		result->set_level(spdlog::level::trace);
		return result;
	}

	void MCLogHelper::SetDefaultLoggers() {
		SetGlobalPattern(GetDefaultConsolePattern());
		SetCoreLogger(CreateStdLoggerMT (DEFAULT_CORE_LOGGER_NAME));
		SetXMLLogger (CreateStdLoggerMT (DEFAULT_XML_LOGGER_NAME));
		SetInitLogger(CreateDualLoggerMT(DEFAULT_INIT_LOGGER_NAME, Paths::InitLogFilename()));
		SetRenderLogger(CreateStdLoggerMT(DEFAULT_RENDER_LOGGER_NAME));
	}

	void MCLogHelper::SetNullLoggers() {
		auto nullSink   = std::make_shared<spdlog::sinks::null_sink_mt>();
		auto nullLogger = std::shared_ptr<spdlog::logger>(new spdlog::logger("null", { nullSink }));
		SetCoreLogger(nullLogger);
		SetXMLLogger (nullLogger);
		SetInitLogger(nullLogger);
		SetRenderLogger(nullLogger);
	}

	void MCLogHelper::SetGlobalPattern(const char *pattern) {
		spdlog::set_pattern(pattern);
	}

}