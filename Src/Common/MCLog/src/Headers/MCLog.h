#pragma once

namespace MC {

	enum MC_LOG_LEVEL {
		trace   = 0,
		info    = 1,
		warning = 2,
		error   = 3,
		fatal   = 4
	};

	class MCLog
	{
	public:
		static MCLog& GetInstance() {
			static MCLog instance;
			return instance;
		}

		void Log(const char* const logger, MC_LOG_LEVEL level, const char* const msg) const;

		~MCLog();
	private:

		// Singleton... private constructor.
		MCLog();

	public:
		MCLog(MCLog const&)          = delete;
		void operator=(MCLog const&) = delete;
	};

}

#ifndef __DISABLE_MC_ERROR
#define MC_ERROR(msg) MC::MCLog::GetInstance().Log("standard", MC::MC_LOG_LEVEL::error, (msg))
#else
#define MC_ERROR(mgs)
#endif // !__DISABLE_MC_ERROR

#ifndef __DISABLE_MC_WARNING
#define MC_WARN(msg) MC::MCLog::GetInstance().Log("standard", MC::MC_LOG_LEVEL::warning, (msg))
#else
#define MC_WARN(msg)
#endif // !__DISABLE_MC_WARNING

#ifndef __DISABLE_MC_INFO
#define MC_INFO(msg) MC::MCLog::GetInstance().Log("standard", MC::MC_LOG_LEVEL::info, (msg))
#else
#define MC_INFO(msg)
#endif // !__DISABLE_MC_INFO


#ifdef __ENABLE_MC_TRACE
#define MC_TRACE(msg) MC::MCLog::GetInstance().Log("standard", MC::MC_LOG_LEVEL::trace, (msg))
#else
#define MC_TRACE(msg)
#endif // __ENABLE_MC_TRACE