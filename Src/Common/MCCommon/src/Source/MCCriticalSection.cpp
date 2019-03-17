#include "../Headers/MCCriticalSection.h"
#include "../Headers/Utility.h"
#include "assert.h"
#include <unordered_map>
#include <string>
#include <thread>


namespace MC {

#pragma region Debug MCCritical Section

#ifdef __DEBUG_MCCRITICAL_SECTION__

	class MCCriticalSectionTracker {
	public:

		MCCriticalSectionTracker() {
			_trackerLock.store(0);
		}

		void Track(const std::string &criticalSectionName) {
			auto thisThreadId = std::this_thread::get_id();
			auto alreadyTracked = false;
			int expected = 0;
			while (!_trackerLock.compare_exchange_strong(expected, 1))
				expected = 0;

			if (_trackedItems.find(thisThreadId) == _trackedItems.end()) {
				_trackedItems[thisThreadId] = criticalSectionName;
				alreadyTracked = true;
			}

			_trackerLock.store(0);

			if(alreadyTracked) {
				static_assert(sizeof(std::thread::id) == sizeof(uint32_t), "this function only works if size of thead::id is equal to the size of uint_64");
				auto id = std::this_thread::get_id();
				uint32_t* ptr = (uint32_t*)&id;
				auto intID = (*ptr);
				throw MCException (
					std::string("Thread ") + std::to_string(intID) +
					std::string(" is attempting to enter critical section ") +
					std::string(criticalSectionName) +
					std::string(" while already in critical section ") +
					_trackedItems[thisThreadId],
					std::string("filename not tracked for this error."),
					-1
				);
			}
		}

		void Untrack() {
			auto id = std::this_thread::get_id();
			int expected = 0;
			while (!_trackerLock.compare_exchange_strong(expected, 1))
				expected = 0;
			if (_trackedItems.find(id) != _trackedItems.end())
				_trackedItems.erase(id);
			_trackerLock.store(0);
		}

	private:
		std::unordered_map<std::thread::id, std::string> _trackedItems;
		std::atomic_int _trackerLock;
	};

	MCCriticalSectionTracker gMCCriticalSectionTracker;

	MCCriticalSection::MCCriticalSection(const char *pName, MCCriticalSectionLock *pLock) 
		: MCCriticalSection(pLock) {
		gMCCriticalSectionTracker.Track(std::string(pName));
	}

#endif __DEBUG_MCCRITICAL_SECTION__

#pragma endregion 

	MCCriticalSection::MCCriticalSection(MCCriticalSectionLock *pLock) {
		_pLock = pLock;

#ifdef __DEBUG_MCCRITICAL_SECTION__
		auto initialValue = _pLock->load();
		assert(initialValue == 0 || initialValue == 1);
#endif __DEBUG_MCCRITICAL_SECTION__

		int expected = 0;
		while (!_pLock->compare_exchange_strong(expected, 1)) {
			expected = 0;
		}
	}

	MCCriticalSection::~MCCriticalSection() {
		_pLock->store(0);
#ifdef __DEBUG_MCCRITICAL_SECTION__
		gMCCriticalSectionTracker.Untrack();
#endif __DEBUG_MCCRITICAL_SECTION__
	}

	void MCCriticalSection::InitializeLock(MCCriticalSectionLock *pLock) {
		pLock->store(0);
	}

}