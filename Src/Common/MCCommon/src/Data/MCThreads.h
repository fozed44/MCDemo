#pragma once

#include <string>
#include <thread>
#include <map>
#include <tuple>

#include "../Headers/GlobalSwitches.h"

#ifdef __ENABLE_THREAD_ASSERT__
#define MCTHREAD_ASSERT(x) MC::MCThreads::Assert((x))
#else
#define MCTHREAD_ASSERT(x)
#endif __ENABLE_THREAD_ASSERT__

namespace MC {

	enum MC_THREAD_CLASS {
		MC_THREAD_CLASS_MAIN = 0,
		MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER = 1
	};

	class MCThreads {
	public:
		static void RegisterCurrentThread(const std::string& name, MC_THREAD_CLASS threadClass);
		static void Assert(const std::string& name);
		static void Assert(MC_THREAD_CLASS threadClass);
		static int  ID();
		static std::string     Name();
		static MC_THREAD_CLASS ThreadClass();
	private:
		static std::map<std::thread::id, std::tuple<std::string, MC_THREAD_CLASS>> _threadMap;
	};

}