#pragma once

#include <string>
#include "spdlog/spdlog.h"

namespace MC {

	class MCLog
	{
	public:
		static MCLog& GetInstance() {
			static MCLog instance;
			return instance;
		}

		inline std::shared_ptr<spdlog::logger> GetCore() { return _pCoreLogger; }
		inline std::shared_ptr<spdlog::logger> GetXml()  { return _pXMLLogger; }
		inline std::shared_ptr<spdlog::logger> GetInit() { return _pInitLogger; }

		~MCLog();
	private:

		// Singleton... private constructor.
		MCLog();
		std::shared_ptr<spdlog::logger> _pCoreLogger;
		std::shared_ptr<spdlog::logger> _pXMLLogger;
		std::shared_ptr<spdlog::logger> _pInitLogger;

		std::string GetExecutingDirectory() const;
		std::string GetInitLogFilename() const;
	public:
		MCLog(MCLog const&)          = delete;
		void operator=(MCLog const&) = delete;
	};

}


//////////////////////////////////////////////////////////////////////////////
// CORE LOGGER 

#ifndef __DISABLE_MC_ERROR
#define MC_ERROR(...) MC::MCLog::GetInstance().GetCore()->error(__VA_ARGS__)
#else
#define MC_ERROR(...)
#endif // !__DISABLE_MC_ERROR

#ifndef __DISABLE_MC_WARNING
#define MC_WARN(...) MC::MCLog::GetInstance().GetCore()->warn(__VA_ARGS__)
#else
#define MC_WARN(...)
#endif // !__DISABLE_MC_WARNING

#ifndef __DISABLE_MC_INFO
#define MC_INFO(...) MC::MCLog::GetInstance().GetCore()->info(__VA_ARGS__)
#else
#define MC_INFO(...)
#endif // !__DISABLE_MC_INFO

#ifdef __ENABLE_MC_TRACE
#define MC_TRACE(...) MC::MCLog::GetInstance().GetCore()->trace(__VA_ARGS__)
#else
#define MC_TRACE(...)
#endif // __ENABLE_MC_TRACE

//////////////////////////////////////////////////////////////////////////////
// XML LOGGER

#ifndef __DISABLE_XML_ERROR
#define XML_ERROR(...) MC::MCLog::GetInstance().GetXml()->error(__VA_ARGS__)
#else
#define XML_ERROR(...)
#endif // !__DISABLE_XML_ERROR

#ifndef __DISABLE_XML_WARNING
#define XML_WARN(...) MC::MCLog::GetInstance().GetXml()->warn(__VA_ARGS__)
#else
#define XML_WARN(...)
#endif // !__DISABLE_XML_WARNING

#ifndef __DISABLE_XML_INFO
#define XML_INFO(...) MC::MCLog::GetInstance().GetXml()->info(__VA_ARGS__)
#else
#define XML_INFO(...)
#endif // !__DISABLE_XML_INFO

#ifdef __ENABLE_XML_TRACE
#define XML_TRACE(...) MC::MCLog::GetInstance().GetXml()->trace(__VA_ARGS__)
#else
#define XML_TRACE(...)
#endif // __ENABLE_XML_TRACE

//////////////////////////////////////////////////////////////////////////////
// INIT LOGGER

#ifndef __DISABLE_INIT_ERROR
#define INIT_ERROR(...) MC::MCLog::GetInstance().GetInit()->error(__VA_ARGS__)
#else
#define INIT_ERROR(...)
#endif // !__DISABLE_INIT_ERROR

#ifndef __DISABLE_INIT_WARNING
#define INIT_WARN(...) MC::MCLog::GetInstance().GetInit()->warn(__VA_ARGS__)
#else
#define INIT_WARN(...)
#endif // !__DISABLE_XML_WARNING

#ifndef __DISABLE_INIT_INFO
#define INIT_INFO(...) MC::MCLog::GetInstance().GetInit()->info(__VA_ARGS__)
#else
#define INIT_INFO(...)
#endif // !__DISABLE_XML_INFO

#ifdef __ENABLE_INIT_TRACE
#define INIT_TRACE(...) MC::MCLog::GetInstance().GetInit()->trace(__VA_ARGS__)
#else
#define INIT_TRACE(...)
#endif // __ENABLE_XML_TRACE
