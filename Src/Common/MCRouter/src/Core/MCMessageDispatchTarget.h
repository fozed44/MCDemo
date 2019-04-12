#pragma once

#include "MCMessage.h"

namespace MC {

	class MCMessageDispatchTarget {
	public: /* ctor */
		MCMessageDispatchTarget() {}
		virtual ~MCMessageDispatchTarget() {}
	public:
		virtual void ProcessMessage32 (		 MC_MESSAGE32   message) {}
		virtual void ProcessMessage64 (		 MC_MESSAGE64   message) {}
		virtual void ProcessMessage128(const MC_MESSAGE128& message) {}
		virtual void ProcessMessage128(const MC_MESSAGE128& message, const char* pData) {}
	};
}