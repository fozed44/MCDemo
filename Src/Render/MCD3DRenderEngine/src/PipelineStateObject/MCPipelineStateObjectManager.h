#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"
#include "../../../../Common/MCCommon/src/Data/MCResult.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	typedef enum class STANDARD_PSO {
		DEFAULT = 0
	} STANDARD_PSO;

	class MCPipelineStateObjectManager : public MCManagedHandleManager<ID3D12PipelineState*, ComPtr<ID3D12PipelineState>, MCPipelineStateObjectManager> {
	public: /* Handle type alias */
		using HPipelineState = MCPipelineStateObjectManager::HandleType;
	public: /* ctor */
		MCPipelineStateObjectManager();
		~MCPipelineStateObjectManager();
		MCPipelineStateObjectManager(MCPipelineStateObjectManager&)              = delete;
		MCPipelineStateObjectManager(MCPipelineStateObjectManager&&)             = delete;
		MCPipelineStateObjectManager& operator= (MCPipelineStateObjectManager&)  = delete;
		MCPipelineStateObjectManager& operator= (MCPipelineStateObjectManager&&) = delete;

	public: /* Instance access */
		static MCPipelineStateObjectManager* Instance() { return MCREGlobals::pPSOManager; }

	public: /* API */
		/* Manage a custom PSO. */
		HPipelineState CreatePSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const char *pName);

		/* Get a handle to the specified standardPso. If the pso does not already 
		   exist in the internal standard pso map, it is loaded before the method
		   returns. */
		HPipelineState GetPipelineStateHandle(STANDARD_PSO standardPSO);

		/* Get a PSO from an HPipelineState. */
		ID3D12PipelineState *GetPipelineState(const HPipelineState& hPipelineState);

		/* Clear all loaded PSOs currently associated with the standard pso map.*/
		MC_RESULT Clear();
	private:
		std::map<STANDARD_PSO, ComPtr<ID3D12PipelineState>> _standardPSOMap;

		/*	Hard codes a mapping between STANDARD_PSO values and the functions that create
			the ID3D12PipelineState for those values. */
		ComPtr<ID3D12PipelineState> GetStandardPiplineState(STANDARD_PSO);
	};

	using HPipelineState = MCPipelineStateObjectManager::HPipelineState;
}