#pragma once

#include "MCMessage.h"
#include "../../../MCCommon/src/Headers/MCCriticalSection.h"
#include "MCMessageQueue.h"
#include "MCMessageDispatchTarget.h"

#include <vector>

/*

	*----------------------------------------------------------------*
	|                                       MCRouter                                      |
	|----------------------------------------------------------------|
	|  *--------------------------------------*                               |
	|  | MCMessageQueue: _pMessageQueue32 |                               |
	|  |  frontQueue                   backQueue    |                               |
	|  *--------------------------------------*                               |
	|  *--------------------------------------*                               |
	|  | MCMessageQueue: _pMessageQueue64 |                               |
	|  |  frontQueue                   backQueue    |                               |  
	|  *--------------------------------------*                               |
	|  *---------------------------------------------------------*    |
	|  | MCMessageQueueWithMemoryBuffer: _pMessageQueue128 |     |
	|  |  frontQueue                             backQueue                   |      |
	|  |  frontBuffer                              backBuffer                    |     |
	|  *---------------------------------------------------------*    |
	|  *----------------*    *------------------*                             |
	|  | DispatchTargets |    | read / write locks |                              |
	|  *----------------*    *------------------*                             |
	*----------------------------------------------------------------*

	MCRouter:

	Purpose:
		The MCRouter is the backbone of the messaging system in this framework.
		
	Remarks:
		The messaging system is built on three ideas / systems.

		1) It provides a method for any thread to post a message, conversely all messages must be posted to the
		   router.

		2) It provides a method for any MessageDispatchTarget to register in order to be notified
		   of messages.

		3) Provides the update method which:
			a) Flips the front and back queues, and in the case of the 128 bit message queue the buffer
			   is also flipped, and then runs the dispatching code on the new back buffer.

		Notes:

		1) Writes are posted to front buffers.

		2) Reads are read from back buffers.

		3) Writes not made from the main thread should happen in a write lock.

		4) Reads not made from the main thread should happen in a read lock.

		5) Read / Write locks are used to synchronize the queue/buffer swaps.

		6) Use message visibility flags to prevent irrelevant dispatch targets from having to
		   needlessly processes worthless messages.

		7) Use the PostConsoleMessage() as a helper method to post a console message.

*/

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

	public: /* Update */

		void Update();

	private: /* Swap / Dispatch */

		MC_RESULT Swap();
		void Dispatch();

	public: /* RegisterDispatchTargets */

		void RegisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget, MC_MESSAGE_VISIBILITY visibility);
		void UnregisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget);

	public: /* Helper Methods */

		/* Call PostConsoleMessage from the main thread, or from within write locks. */
		void PostConsoleMessage(const char* pMsg);

		/* Call PostConsoleMessageLocked from outside of the main thread, NOT within write locks. */
		void PostConsoleMessageLocked(const char* pMsg);

	private: /* Private Methods */

		void DispatchMessages32();
		void DispatchMessages64();
		void DispatchMessages128();

	private: /* Private Data */

		std::unique_ptr<MCMessageQueue<MC_MESSAGE32,  ROUTER_QUEUE_SIZE>> _pMessageQueue32;
		std::unique_ptr<MCMessageQueue<MC_MESSAGE64,  ROUTER_QUEUE_SIZE>> _pMessageQueue64;
		std::unique_ptr<MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, ROUTER_QUEUE_SIZE, ROUTER_BUFFER_SIZE>> _pMessageQueue128;

		std::vector<std::pair<MCMessageDispatchTarget*, MC_MESSAGE_VISIBILITY>> _dispatchTargets;
		
		std::vector<std::thread::id> _writeLocks;
		std::vector<std::thread::id> _readLocks;
		MCCriticalSectionLock _lock;
	};
}

