#include "D3D12RenderControlDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCMessage.h"
#include "../../../../Common/MCCommon/src/Console/MCConsoleCommand.h"
#include "../Render/MCRenderer.h"

#include <string>
#include <algorithm>

namespace MC {

#pragma region ctor

	D3D12RenderControlDispatchTarget::D3D12RenderControlDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter) 
		: MCMessageDispatchTarget(pParent, MC_MESSAGE_VISIBILITY_SYSTEM),
		  _pRouter{ pRouter } {}

#pragma endregion

#pragma region OnProcessMessage128

	void D3D12RenderControlDispatchTarget::OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) {
		if (message.Message == MC_MESSAGE_CONSOLE_COMMAND_128) {

			auto cmd = reinterpret_cast<MC_CONSOLE_COMMAND*>(message.pAddress);

			switch (message.LOParam32) {
			case MC_CONSOLE_COMMAND_CMD_RENDERER:
				HandleRendererCommand(cmd->ParameterCount, cmd->pParameterData);
				break;
			}
		}
	}

#pragma endregion

#pragma region COMMAND: Renderer

	void HandleSetStateRendererParameter(char *pParam, MCRouter* pRouter) {

		auto cmd = std::string(pParam);
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

		if (cmd == "off") {
			pRouter->Push(MC_MESSAGE32{
				MC_MESSAGE_SET_RENDERER_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)MC_RENDER_STATE_OFF
			});
		}

		if (cmd == "space") {
			pRouter->Push(MC_MESSAGE32{
				MC_MESSAGE_SET_RENDERER_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)MC_RENDER_STATE_SPACE
			});
		}
	}

	void HandleFirstRendererParameter(char **pParams, unsigned char totalParams, MCRouter* pRouter) {
		auto cmd = std::string(pParams[0]);
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

		if (cmd == "pause") {
			pRouter->Push(MC_MESSAGE32 { 
				MC_MESSAGE_SET_PAUSE_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)1
			});
		}
		else if (cmd == "unpause") {
			pRouter->Push(MC_MESSAGE32 {
				MC_MESSAGE_SET_PAUSE_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)0
			});
		}
		else if (cmd == "set-state") {
			// TODO Add a warning message if we are short parameters
			if (totalParams < 2)
				return;
			HandleSetStateRendererParameter(pParams[1], pRouter);
		}
	}

	/* Handle the *renderer* console command.
	*/
	void D3D12RenderControlDispatchTarget::HandleRendererCommand(unsigned char parameterCount, char **pParameterData) {
		// If the command does not have a parameter then *renderer* doesn't do anything.
		if (!parameterCount)
			return;
	}

#pragma endregion

}