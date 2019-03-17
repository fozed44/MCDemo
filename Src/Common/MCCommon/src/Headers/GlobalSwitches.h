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