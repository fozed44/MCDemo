#include "D3D12RenderEngineDispatchTarget.h"
#include "../Core/MCREGlobals.h"
#include "../Core/MCD3D12RenderEngine.h"

#include "stdio.h"

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
			MCREGlobals::pRenderEngine->SetPauseState(true);
		}
		else {
			_pRouter->PostConsoleMessage("Renderer Unpaused.");
			MCREGlobals::pRenderEngine->SetPauseState(false);
		}
	}

	void D3D12RenderEngineDispatchTarget::SetRenderState(MC_RENDER_STATE renderState) {
		auto result = MCREGlobals::pRenderEngine->SetRenderState(renderState);
		if (result != MC_RESULT::OK) {
			RENDER_WARN("SetRenderState failed {0}.", to_string(result));
			_pRouter->PostConsoleMessage((std::string("Failed to set render state: ") + to_string(result)).c_str());
		}
	}

	void D3D12RenderEngineDispatchTarget::OnProcessMessage32(MC_MESSAGE32 message) {
		if (message.Message == MC_MESSAGE_SET_PAUSE_STATE_32) {
			SetPauseState(static_cast<bool>(message.Param));
		} 
		else if (message.Message == MC_MESSAGE_SET_RENDERER_STATE_32) {
			SetRenderState(static_cast<MC_RENDER_STATE>(message.Param));
		}
	}

#pragma endregion

}