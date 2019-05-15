#pragma once

#include "MCMessage.h"
#include "assert.h"
#include <memory>
#include <vector>

namespace MC {

	class MCMessageDispatchTarget {
	public: /* ctor */
		MCMessageDispatchTarget() : _pParent{ nullptr } {}
		MCMessageDispatchTarget(MCMessageDispatchTarget* pParent, MC_MESSAGE_VISIBILITY visibility) {
			assert(pParent);
			pParent->RegisterDispatchTarget(this, visibility);
			_pParent = pParent;
		}
		virtual ~MCMessageDispatchTarget() { if (_pParent) _pParent->UnregisterDispatchTarget(this); }
	public: /* Default Message Processing */
		        void ProcessMessage32   (      MC_MESSAGE32   message);
		        void ProcessMessage64   (      MC_MESSAGE64   message);
		        void ProcessMessage128  (const MC_MESSAGE128& message);
		        void ProcessMessage128  (const MC_MESSAGE128& message, const char* pData);

		virtual void OnProcessMessage32 (	   MC_MESSAGE32   message) {}
		virtual void OnProcessMessage64 (	   MC_MESSAGE64   message) {}
		virtual void OnProcessMessage128(const MC_MESSAGE128& message) {}
		virtual void OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) {}
	public: /* Support for dispatch-sub-targets. */
		void RegisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget, MC_MESSAGE_VISIBILITY visibility);
		void UnregisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget);
	private:
		MCMessageDispatchTarget* _pParent; // Note that a dispatch target is not required to have a parent.
		std::vector<std::pair<MCMessageDispatchTarget*, MC_MESSAGE_VISIBILITY>> _dispatchTargets;
	};
}