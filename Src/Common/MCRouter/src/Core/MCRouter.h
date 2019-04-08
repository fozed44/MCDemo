#pragma once

#include "MCMessage.h"
#include "../../../MCCommon/src/Headers/MCCriticalSection.h"

namespace MC {

	const int ROUTER_BUFFER_SIZE = sizeof(MC_MESSAGE128) * 1000;

	class MCRouter {
	public: /* ctor */
		MCRouter();
		~MCRouter();
		MCRouter(MCRouter&)             = delete;
		MCRouter(MCRouter&&)            = delete;
		MCRouter& operator=(MCRouter&)  = delete;
		MCRouter& operator=(MCRouter&&) = delete;

	public:
		void Post(MC_MESSAGE    message);
		void Post(MC_MESSAGE32  message);
		void Post(MC_MESSAGE64  message);
		void Post(MC_MESSAGE128 message);

		void Peek()

	private:
		void InitializeBuffer(size_t bytes);

		char* Allocate(size_t bytes);
		void  Release();

		char* _pBase;
		char* _pEnd;
		char* _pNext;

		MCCriticalSectionLock _lock;
	};
}

