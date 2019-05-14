#include "MCMessageDispatchTarget.h"

namespace MC {

#pragma region Default Message Processing

	void MCMessageDispatchTarget::ProcessMessage32(MC_MESSAGE32 message) {
		OnProcessMessage32(message);
		for (auto& t : _dispatchTargets)
			if(message.Visibility & t.second)
				t.first->ProcessMessage32(message);
	}

	void MCMessageDispatchTarget::ProcessMessage64(MC_MESSAGE64 message) {
		OnProcessMessage64(message);
		for (auto& t : _dispatchTargets)
			if (message.Visibility & t.second)
				t.first->ProcessMessage64(message);
	}

	void MCMessageDispatchTarget::ProcessMessage128(const MC_MESSAGE128& message) {
		OnProcessMessage128(message);
		for (auto& t : _dispatchTargets)
			if (message.Visibility & t.second)
				t.first->ProcessMessage128(message);
	}

	void MCMessageDispatchTarget::ProcessMessage128(const MC_MESSAGE128& message, const char * pData) {
		OnProcessMessage128(message, pData);
		for (auto& t : _dispatchTargets)
			if (message.Visibility & t.second)
				t.first->ProcessMessage128(message, pData);
	}

#pragma endregion

#pragma region AddDispatchTarget

	void MCMessageDispatchTarget::RegisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget, MC_MESSAGE_VISIBILITY visibility) {
		_dispatchTargets.push_back({ pDispatchTarget, visibility });
	}

	void MCMessageDispatchTarget::UnregisterDispatchTarget(MCMessageDispatchTarget* pDispatchTarget) {
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

}