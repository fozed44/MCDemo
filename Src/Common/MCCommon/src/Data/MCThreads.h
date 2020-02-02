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

	enum class MC_THREAD_CLASS {
		MAIN              = 0,
		RENDERER_EXECUTER = 1
	};

	/*
		Class: MCThreads

		Purpose:
			Use MCThreads to associate a name and a MC_THREAD_CLASS with a thread. The class then provides methods
			to perform thread based assertions.

			For example use RegisterCurrentThread("render", MC_THREAD_CLASS.RENDERER_EXECUTER) to associate the name and class with
			the current thread, enabling calles to Assert("render") or Assert (MC_THREAD_CLASS) (Hoever these methods should
			not normally be called directly, instead use the MCTHREAD_ASSERT macro to enable define switches).
	*/
	class MCThreads {
	public:
		/*	Register'name' and 'threadClass with the current_thread. 'name' must be unique. */
		static void RegisterCurrentThread(const std::string& name, MC_THREAD_CLASS threadClass);

		/* Assert that the current thread has 'name'. */
		static void Assert(const std::string& name);

		/* Assert that the current thread has 'threadClass' */
		static void Assert(MC_THREAD_CLASS threadClass);

		/* Assert that thread 'id' has been registered. */
		static void AssertRegistered(std::thread::id id);

		/* Assert that a thread has been registered with 'name' */
		static void AssertRegistered(const std::string& name);

		/* Assert that the current thread has been registered. */
		static void AssertRegistered();

		/* Get the id of the current thread. */
		static int  ID();

		/* Get the name associated with the current thread. The current thread must be registered. */
		static std::string     Name();

		/* Get the class of the current thread. The current thread must be registered. */
		static MC_THREAD_CLASS ThreadClass();
	private:
		static std::map<std::thread::id, std::tuple<std::string, MC_THREAD_CLASS>> _threadMap;
	};

}