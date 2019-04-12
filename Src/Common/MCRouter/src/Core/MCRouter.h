#pragma once

#include "MCMessage.h"
#include "../../../MCCommon/src/Headers/MCCriticalSection.h"
#include "MCMessageQueue.h"
#include "MCMessageDispatchTarget.h"

#include <vector>

namespace MC {

	const int ROUTER_BUFFER_SIZE = 32*1024;
	const int ROUTER_QUEUE_SIZE  = 200;

	class MCRouter {
	public: /* ctor */
		MCRouter();
		~MCRouter();
		MCRouter(MCRouter&)             = delete;
		MCRouter(MCRouter&&)            = delete;
		MCRouter& operator=(MCRouter&)  = delete;
		MCRouter& operator=(MCRouter&&) = delete;

	public: /* Message Pushing */
		void Push(const MC_MESSAGE32&   message);
		void Push(const MC_MESSAGE64&   message);
		void Push(const MC_MESSAGE128&  message);

		char* PushTo(const MC_MESSAGE128& message, unsigned short dataSize);

		template<typename T>
		T* PushTo(const MC_MESSAGE128& message) {
			return _pMessageQueue128->push_to<T>(message);
		}

		char* AllocateMessageStorage(unsigned short size) {
			return _pMessageQueue128->allocate(size);
		}

	public: /* Read Write Locks */

		void AddReadLock();
		void ReleaseReadLock();

		void AddWriteLock();
		void ReleaseWriteLock();

	public: /* Swap / Dispatch */

		void Swap();
		void Dispatch();

	public: /* RegisterDispatchTargets */

		void RegisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget, MC_MESSAGE_VISIBILITY visibility);

	private:

		void DispatchMessages32();
		void DispatchMessages64();
		void DispatchMessages128();

		std::unique_ptr<MCMessageQueue<MC_MESSAGE32,  ROUTER_QUEUE_SIZE, 0>> _pMessageQueue32;
		std::unique_ptr<MCMessageQueue<MC_MESSAGE64,  ROUTER_QUEUE_SIZE, 0>> _pMessageQueue64;
		std::unique_ptr<MCMessageQueue<MC_MESSAGE128, ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>> _pMessageQueue128;

		std::vector<MCMessageDispatchTarget*> _dispatchTargets;
		std::vector<MC_MESSAGE_VISIBILITY>    _dispatchTargetVisibility;
		
	};
}

