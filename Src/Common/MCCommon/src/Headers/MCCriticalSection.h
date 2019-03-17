#pragma once

#include <atomic>
#include "GlobalSwitches.h"

/*
	When __DEBUG_MCCRITICIAL_SECTION is defined, ENTER_CRITICAL_SECTION will call an overload of the MCCriticalSection
	that will track every time a thread enters a critical section. This tracking is not efficient, but will allow the tracker to throw
	an exception if a thread attempts to enter a critical section if that thread is already in a critical section.
*/
#ifdef __DEBUG_MCCRITICAL_SECTION__
#define ENTER_CRITICAL_SECTION(sectionName, pLock) try { MC::MCCriticalSection sectionName((#sectionName), (pLock));
#define EXIT_CRITICAL_SECTION } \
catch (MCException *ex) { MessageBox(nullptr, ex->what().c_str(), "MCException", MB_OK); } \
catch (MCException ex) { \
	MessageBox(nullptr, ex.what().c_str(), "MCException", MB_OK); \
}
#else
#define ENTER_CRITICAL_SECTION(sectionName, pLock) { MC::MCCriticalSection sectionName(pLock);
#define EXIT_CRITICAL_SECTION }
#endif __DEBUG_MCCRITICAL_SECTION__


namespace MC {

	/*
		MCCriticalSectionLock abstracts the lock type.

		Clients should alway use MCCriticalSection::InitializeLock to initialize locks. This will prevent clients to become
		dependant on the underlying type backing MCCriticalSectionLock.
	*/
	typedef std::atomic_int MCCriticalSectionLock;

	/*
		An MCCriticalSection is a pure RAII resource.

		The constructor blocks until the critical section has been entered. The critical section is left when the object
		goes out of scope.

		Since MCCriticalSection is pure RAII depending purely on ctor/dtor for the lifetime of the critical section, an
		MCCriticalSection should normally only be created on the stack.
	*/
	class MCCriticalSection {
	public:
#ifdef __DEBUG_MCCRITICAL_SECTION__
		MCCriticalSection(const char *pName, MCCriticalSectionLock *pLock);
#endif __DEBUG_MCCRITICAL_SECTION__
		MCCriticalSection(MCCriticalSectionLock *pLock);
		~MCCriticalSection();
		MCCriticalSection(MCCriticalSection&)             = delete;
		MCCriticalSection& operator= (MCCriticalSection&) = delete;
		MCCriticalSection(MCCriticalSection&&)            = delete;

		/*
			Clients should always use InitializeLock to limit client exposure to the type backing MCCriticalSectionLock.
		*/
		static void InitializeLock(MCCriticalSectionLock *pLock);

	private:
		MCCriticalSectionLock *_pLock;
	};

}