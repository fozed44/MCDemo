#include "MCRouter.h"

namespace MC {

#pragma region ctor

	MCRouter::MCRouter(){

		MCCriticalSection::InitializeLock(&_lock);

		_pMessageQueue32  = std::make_unique<MCMessageQueue<MC_MESSAGE32,  ROUTER_QUEUE_SIZE>>("Message Queue 32");
		_pMessageQueue64  = std::make_unique<MCMessageQueue<MC_MESSAGE64,  ROUTER_QUEUE_SIZE>>("Message Queue 64");
		_pMessageQueue128 = std::make_unique<MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>>("Message Queue 128");
	}


	MCRouter::~MCRouter() { }

#pragma endregion

#pragma region Push handlers

	void MCRouter::Push(const MC_MESSAGE32& message) {
		_pMessageQueue32->push(message);
	}

	void MCRouter::Push(const MC_MESSAGE64& message) {
		_pMessageQueue64->push(message);
	}

	void MCRouter::Push(const MC_MESSAGE128& message) {
		_pMessageQueue128->push(message);
	}

	char* MCRouter::PushTo(const MC_MESSAGE128& message, unsigned short dataSize) {
		return _pMessageQueue128->push_to(message, dataSize);
	}

#pragma endregion

#pragma region Dispatch Target Registration

	void MCRouter::RegisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget, MC_MESSAGE_VISIBILITY visibility) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		_dispatchTargets.push_back({ pDispatchTarget, visibility });
	}

	void MCRouter::UnregisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		auto size = _dispatchTargets.size();
		for (unsigned int i = 0; i < size; i++) {
			if (&(*_dispatchTargets[i].first) == &(*pDispatchTarget)) {
				_dispatchTargets.erase(_dispatchTargets.begin() + i);

				// Once the dispatch target has been removed, we MUST exit this loop,
				// the index positions for 'i' are now invalid.
				return;
			}
		}
	}

#pragma endregion

#pragma region Dispatchers

	void MCRouter::DispatchMessages32() {
		MC_MESSAGE32 msg;
		while (_pMessageQueue32->pop(&msg)) {
			for (auto& t : _dispatchTargets) {
				if (msg.Visibility & t.second)
					t.first->ProcessMessage32(msg);
			}
		}
	}

	void MCRouter::DispatchMessages64() {
		MC_MESSAGE64 msg;
		while (_pMessageQueue64->pop(&msg)) {
			for (auto& t : _dispatchTargets) {
				if (msg.Visibility & t.second)
					t.first->ProcessMessage64(msg);
			}
		}
	}

	void MCRouter::DispatchMessages128() {
		MC_MESSAGE128 msg;
		while (_pMessageQueue128->pop(&msg)) {
			for (auto& t : _dispatchTargets) {
				if (!(msg.Visibility & t.second))
					continue;
				if (msg.Message & MC_MESSAGE_BIT_FLAGS_ADDRESSED) {
					t.first->ProcessMessage128(msg, msg.pAddress);
				}
				else {
					t.first->ProcessMessage128(msg);
				}
			}
		}
	}

#pragma endregion

#pragma region Read Write Locks

	void MCRouter::AddReadLock() {
		ENTER_CRITICAL_SECTION(MCMessageQueue_add_read_lock, &_lock);

#ifdef __DEBUG_ROUTING__
			MCThreads::AssertRegistered();

			if (std::find(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()) != _readLocks.end()) {
				MCTHROW("MCMessageQueue::add_read_lock->threadId already exists.");
			}
#endif __DEBUG_ROUTING__

			_readLocks.push_back(std::this_thread::get_id());
		EXIT_CRITICAL_SECTION;
	}

	void MCRouter::ReleaseReadLock() {
		ENTER_CRITICAL_SECTION(MCMessageQueue_release_read_lock, &_lock);

#ifdef __DEBUG_ROUTING__
			MCThreads::AssertRegistered();

			if (std::find(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()) == _readLocks.end()) {
				MCTHROW("MCMessageQueue::release_read_lock->threadId does not exist.");
			}
#endif __DEBUG_ROUTING__

			_readLocks.erase(std::remove(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()), _readLocks.end());
		EXIT_CRITICAL_SECTION;
	}

	void MCRouter::AddWriteLock() {
		ENTER_CRITICAL_SECTION(MCMessageQueue_add_read_lock, &_lock);

#ifdef __DEBUG_ROUTING__
			MCThreads::AssertRegistered();

			if (std::find(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()) != _writeLocks.end()) {
				MCTHROW("MCMessageQueue::add_write_lock->threadId already exists.");
			}
#endif __DEBUG_ROUTING__

			_writeLocks.push_back(std::this_thread::get_id());
		EXIT_CRITICAL_SECTION;
	}

	void MCRouter::ReleaseWriteLock() {
		ENTER_CRITICAL_SECTION(MCMessageQueue_release_read_lock, &_lock);

#ifdef __DEBUG_ROUTING__
			MCThreads::AssertRegistered();

			if (std::find(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()) == _writeLocks.end()) {
				MCTHROW("MCMessageQueue::release_write_lock->threadId does not exist.");
			}
#endif __DEBUG_ROUTING__

			_writeLocks.erase(std::remove(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()), _writeLocks.end());
		EXIT_CRITICAL_SECTION;
	}


#pragma endregion

#pragma region Console Message Helpers

	void MCRouter::PostConsoleMessage(const char* pMsg) {
		auto len = strlen(pMsg) + 1;
		auto ptr = PushTo({
			MC_MESSAGE_CONSOLE_OUTPUT_128,
			MC_MESSAGE_VISIBILITY_CONSOLE },
			static_cast<unsigned short>(len)
		);
		// TODO
		//	This could be an issue. len = strlen + 1, im not sure how strcopy_s copies...
		//  does it implicitly include a null terminator?
		strcpy_s(ptr, len, pMsg);
	}

	void MCRouter::PostConsoleMessageLocked(const char* pMsg) {
		AddWriteLock();
		PostConsoleMessage(pMsg);
		ReleaseWriteLock();
	}

#pragma region

#pragma region Update

	void MCRouter::Update() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		while (MC_RESULT::OK != Swap()) {}
		Dispatch();
	}

#pragma endregion

#pragma region Swap / Dispatch

	MC_RESULT MCRouter::Swap() {
		ENTER_CRITICAL_SECTION(MCMessageQueue_Swap, &_lock);
			size_t writeLocks = _writeLocks.size(); size_t readLocks = _readLocks.size();

			if (writeLocks || readLocks) {
				MC_WARN("MCRouter: can't swap, ({0:d}) write locks, ({1:d}) read locks.", writeLocks, readLocks);
				return MC_RESULT::FAIL_NOT_READY;
			}
			
			_pMessageQueue32 ->swap();
			_pMessageQueue64 ->swap();
			_pMessageQueue128->swap();

			return MC_RESULT::OK;
		EXIT_CRITICAL_SECTION;
	}

	void MCRouter::Dispatch() {
		DispatchMessages32();
		DispatchMessages64();
		DispatchMessages128();
	}

#pragma endregion

}
