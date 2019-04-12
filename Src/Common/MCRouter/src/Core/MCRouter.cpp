#include "MCRouter.h"

namespace MC {

#pragma region ctor

	MCRouter::MCRouter(){
		_pMessageQueue32  = std::make_unique<MCMessageQueue<MC_MESSAGE32,  ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>>("Message Queue 32");
		_pMessageQueue64  = std::make_unique<MCMessageQueue<MC_MESSAGE64,  ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>>("Message Queue 64");
		_pMessageQueue128 = std::make_unique<MCMessageQueue<MC_MESSAGE128, ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>>("Message Queue 128");
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
		_dispatchTargets.push_back(pDispatchTarget);
		_dispatchTargetVisibility.push_back(visibility);
	}

#pragma endregion

#pragma region Dispatchers

	void MCRouter::DispatchMessages32() {
		MC_MESSAGE32 msg;
		while (_pMessageQueue32->pop(&msg)) {
			for (unsigned int x = 0; x < _dispatchTargets.size(); ++x) {
				if (msg.Visibility & _dispatchTargetVisibility[x])
					_dispatchTargets[x]->ProcessMessage32(msg);
			}
		}
	}

	void MCRouter::DispatchMessages64() {
		MC_MESSAGE64 msg;
		char*        pAddress;
		while (_pMessageQueue64->pop(&msg)) {
			for (unsigned int x = 0; x < _dispatchTargets.size(); ++x) {
				if (msg.Visibility & _dispatchTargetVisibility[x])
					_dispatchTargets[x]->ProcessMessage64(msg);
			}
		}
	}

	void MCRouter::DispatchMessages128() {
		MC_MESSAGE128 msg;
		while (_pMessageQueue128->pop(&msg)) {
			for (unsigned int x = 0; x < _dispatchTargets.size(); ++x) {
				if (!(msg.Visibility & _dispatchTargetVisibility[x]))
					continue;
				if (msg.Message & MC_MESSAGE_BIT_FLAGS_ADDRESSED) {
					_dispatchTargets[x]->ProcessMessage128(msg, msg.pAddress);
				}
				else {
					_dispatchTargets[x]->ProcessMessage128(msg);
				}
			}
		}
	}

#pragma endregion

#pragma region Read Write Locks
	
	void MCRouter::AddReadLock() {
		_pMessageQueue32-> add_read_lock();
		_pMessageQueue64-> add_read_lock();
		_pMessageQueue128->add_read_lock();
	}

	void MCRouter::ReleaseReadLock() {
		_pMessageQueue32-> release_read_lock();
		_pMessageQueue64-> release_read_lock();
		_pMessageQueue128->release_read_lock();
	}

	void MCRouter::AddWriteLock() {
		_pMessageQueue32-> add_write_lock();
		_pMessageQueue64-> add_write_lock();
		_pMessageQueue128->add_write_lock();
	}

	void MCRouter::ReleaseWriteLock() {
		_pMessageQueue32-> release_write_lock();
		_pMessageQueue64-> release_write_lock();
		_pMessageQueue128->release_write_lock();
	}

#pragma endregion

#pragma region Swap / Dispatch

	void MCRouter::Swap() {
		_pMessageQueue32->swap();
		_pMessageQueue64->swap();
		_pMessageQueue128->swap();
	}

	void MCRouter::Dispatch() {
		DispatchMessages32();
		DispatchMessages64();
		DispatchMessages128();
	}

#pragma endregion

}
