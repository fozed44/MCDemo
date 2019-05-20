#include "D3D12RenderSystemDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCMessage.h"
#include "../../../../Common/MCCommon/src/Console/MCConsoleCommand.h"
#include "../Render/MCRenderer.h"
#include "../Memory/MCResourceManagerAnalyzer.h"
#include "../Core/MCDXGI.h"

#include <string>
#include <algorithm>

namespace MC {

#pragma region ctor

	D3D12RenderSystemDispatchTarget::D3D12RenderSystemDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter)
		: MCMessageDispatchTarget(pParent, MC_MESSAGE_VISIBILITY_SYSTEM),
		  _pRouter{ pRouter } {}

#pragma endregion

#pragma region OnProcessMessage128

	void D3D12RenderSystemDispatchTarget::OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) {
		if (message.Message == MC_MESSAGE_CONSOLE_COMMAND_128) {

			auto cmd = reinterpret_cast<MC_CONSOLE_COMMAND*>(message.pAddress);

			switch (static_cast<MC_CONSOLE_COMMAND_CMD>(message.LOParam32)) {
			case MC_CONSOLE_COMMAND_CMD::RENDER:
				HandleRenderCommand(cmd->ParameterCount, cmd->pParameterData);
				break;
			case MC_CONSOLE_COMMAND_CMD::REPORT:
				HandleReportCommand(cmd->ParameterCount, cmd->pParameterData);
				break;
			}

		}
	}

#pragma endregion

#pragma region COMMAND: Renderer

	void HandleFirstRendererParameter(char **pParams, unsigned char totalParams, MCRouter* pRouter) {
		
	}

	/* Handle the *render* console command. */
	void D3D12RenderSystemDispatchTarget::HandleRenderCommand(unsigned char parameterCount, char **pParameterData) {
		// If the command does not have a parameter then *render* doesn't do anything.
		if (!parameterCount) {
			_pRouter->PostConsoleMessage("render: parameter required.");
			return;
		}

		auto param1 = std::string(pParameterData[0]);
		std::transform(param1.begin(), param1.end(), param1.begin(), ::tolower);

		std::string param2;
		if (parameterCount == 2) {
			param2 = std::string(pParameterData[1]);
			std::transform(param2.begin(), param2.end(), param2.begin(), ::tolower);
		}

		/********************* render pause ********************/

		if (param1 == "pause") {
			_pRouter->Push(MC_MESSAGE32 {
				MC_MESSAGE_SET_PAUSE_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)1
			});
		}

		/********************* render unpause ********************/

		else if (param1 == "unpause") {
			_pRouter->Push(MC_MESSAGE32 {
				MC_MESSAGE_SET_PAUSE_STATE_32,
				MC_MESSAGE_VISIBILITY_RENDER,
				(uint8_t)0
			});
		}

		/********************* render set-state ********************/

		else if (param1 == "set-state") {

			if (parameterCount < 2) {
				_pRouter->PostConsoleMessage("render set-state: parameter required.");
				return;
			}

			/***************** render set-state off ****************/

			if (param2 == "off") {
				_pRouter->Push(MC_MESSAGE32 {
					MC_MESSAGE_SET_RENDERER_STATE_32,
					MC_MESSAGE_VISIBILITY_RENDER,
					(uint8_t)MC_RENDER_STATE_OFF
				});
			}

			/***************** render set-state space ****************/

			else if (param2 == "space") {
				_pRouter->Push(MC_MESSAGE32 {
					MC_MESSAGE_SET_RENDERER_STATE_32,
					MC_MESSAGE_VISIBILITY_RENDER,
					(uint8_t)MC_RENDER_STATE_SPACE
				});
			}

			/************** render set-state unknown param**************/

			else {
				_pRouter->PostConsoleMessage("render set-state: unknown parameter.");
			}
		}

		/********************* unrecognized parameter ********************/
		else {
			_pRouter->PostConsoleMessage("Unrecognized parameter.");
		}

	}

	/* Handle the *report" console command. */
	void D3D12RenderSystemDispatchTarget::HandleReportCommand(unsigned char parameterCount, char **pParameterData) {
		// If the command does not have a parameter then *report* doesn't do anything.
		if (!parameterCount) {
			_pRouter->PostConsoleMessage("report: parameter required.");
			return;
		}

		auto param1 = std::string(pParameterData[0]);
		std::transform(param1.begin(), param1.end(), param1.begin(), ::tolower);

		std::string param2;
		if (parameterCount == 2) {
			param2 = std::string(pParameterData[1]);
			std::transform(param2.begin(), param2.end(), param2.begin(), ::tolower);
		}

		/********************* report analyze ********************/

		if (param1 == "analyze") {

			if (parameterCount < 2) {
				_pRouter->PostConsoleMessage("report analyze: parameter required.");
				return;
			}

			/*********** report analyze resource-manager **********/
#ifdef _DEBUG
			if (param2 == "resource-manager") {
				auto analysis = MCREGlobals::pResourceManager->GetAnalyzer()->GetAnalysis();
				_pRouter->PostConsoleMessage(analysis.c_str());
			}
#endif _DEBUG

			/*********** unrecognized parameter **********/
			else {
				_pRouter->PostConsoleMessage("report analyze: unrecognized parameter.");
			}

		}

		/********************* report active-objects ********************/

		else if (param1 == "active-object") {
			if (parameterCount < 2) {
				_pRouter->PostConsoleMessage("report active-object: missing parameter.");
			}

			/*********** report active-object detail **********/

			if (param2 == "detail") {
				_pRouter->PostConsoleMessage("Reporting active object detail.");
				DUMP_LIVE_OBJECTS_DETAIL;
			}

			/*********** report active-object summary **********/

			else if (param2 == "summary") {
				_pRouter->PostConsoleMessage("Report active object summary.");
				DUMP_LIVE_OBJECTS_SUMMARY;
			}

			/****** report active-object unrecognized parameter *****/

			else {
				_pRouter->PostConsoleMessage("report active-object: unrecognized parameter.");
			}
		}
		
		/********************* unrecognized parameter ********************/
		else {
			_pRouter->PostConsoleMessage("report active-object Unrecognized parameter.");
		}
	}

#pragma endregion

}