#include "D3D12RenderEngineDispatchTarget.h"
#include "../Core/MCREGlobals.h"
#include "../Core/MCD3D12RenderEngine.h"

namespace MC {

#pragma region ctor

	D3D12RenderEngineDispatchTarget::D3D12RenderEngineDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter)
		: MCMessageDispatchTarget(pParent, MC_MESSAGE_VISIBILITY_RENDER),
		  _pRouter{ pRouter }{}

#pragma endregion

#pragma region OnProcessMessage32

	void D3D12RenderEngineDispatchTarget::SetPauseState(bool pauseState) {
		if (pauseState) {
			_pRouter->PostConsoleMessage("Renderer Paused.");
		}
		else {
			_pRouter->PostConsoleMessage("Renderer Unpaused.");
		}
	}

	void D3D12RenderEngineDispatchTarget::OnProcessMessage32(MC_MESSAGE32 message) {
		if (message.Message == MC_MESSAGE_SET_PAUSE_STATE_32) {
			SetPauseState(static_cast<bool>(message.Param));
		}
	}

#pragma endregion

}