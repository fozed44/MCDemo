#pragma once

/*
	Include first in translation units to create a common location to turn off/on #define switches.
*/

#define __ENABLE_XML_TRACE
#define __ENABLE_MC_TRACE
#define __ENABLE_INIT_TRACE

/*
	Defining __DEBUG_MCCRITICAL_SECTION will cause the critical section tracker to be included. This tracker
	prevents a thread from going more than on one critical section 'deep'. In other words, each thread can hold
	only a single MCCriticalSectionLock at a time.

	Note:
		This tracker is anything but efficient. It should only be used when actual debugging is needed. Do not
	leave __DEBUG_MCCRITICAL_SECTION__ defined unnecessarily.
*/
//#define __DEBUG_MCCRITICAL_SECTION__

/*
	Defining __MC_THREAD_EXCEPTION_REPORTING__ turns on the wrapping of top level thread methods in try
	catch blocks that will report caught exceptions to windows message boxes.
*/
#define __MC_THREAD_EXCEPTION_REPORTING__

/*
	Defining __ENABLE_THREAD_ASSERT__ causes the MCTHREAD_ASSERT macro to expand to a call to MC::MCThreads::Assert.
	Without this #define, the MCTHREAD_ASSERT macro expands to nothing.
*/
#define __ENABLE_THREAD_ASSERT__