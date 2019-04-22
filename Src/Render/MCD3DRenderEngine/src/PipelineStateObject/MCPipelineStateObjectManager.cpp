#include "MCPipelineStateObjectManager.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../Core/MCDXGI.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "MCPipelineStateObjects.h"
#include "assert.h"

namespace MC {

#pragma region ctor

	MCPipelineStateObjectManager::MCPipelineStateObjectManager() {
		// Ensure a single object instance.
		assert(!MCREGlobals::pPSOManager);

		// Must be running on the main thread
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
	}

	MCPipelineStateObjectManager::~MCPipelineStateObjectManager() {}

#pragma endregion

#pragma region API

	HPipelineState MCPipelineStateObjectManager::CreatePSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const char *pName) {
		RENDER_TRACE("Creating pipeline state object {0}", nullptr == pName ? "n/a" : pName);

		ComPtr<ID3D12PipelineState> pResult;

		MCREGlobals::pMCDXGI->Get3DDevice()->CreateGraphicsPipelineState(
			&desc,
			__uuidof(pResult),
			&pResult
		);

		pResult->SetName(nullptr == pName ? L"n/a" : s2ws(pName).c_str());

		RENDER_TRACE("Created pipeline state object {0}", nullptr == pName ? "n/a" : pName);

		return CreateRef(pResult.Get(), pResult);
	}

	HPipelineState MCPipelineStateObjectManager::GetPipelineStateHandle(STANDARD_PSO standardPSO) {
		auto iterator = _standardPSOMap.find(standardPSO);

		if (iterator != _standardPSOMap.end())
			return CreateRef(iterator->second.Get(), iterator->second);

		auto result = GetStandardPiplineState(standardPSO);

		_standardPSOMap[standardPSO] = result;

		return CreateRef(result.Get(), result);
	}

	ID3D12PipelineState* MCPipelineStateObjectManager::GetPipelineState(const HPipelineState& hPipelineState) {
		return UnwrapHandle(hPipelineState);
	}

	MC_RESULT MCPipelineStateObjectManager::Clear() {
		_standardPSOMap.clear();
	}

	ComPtr<ID3D12PipelineState> MCPipelineStateObjectManager::GetStandardPiplineState(STANDARD_PSO standardPSO) {
		switch (standardPSO) {
		case STANDARD_PSO::DEFAULT:
			return(MC::PSOs::Default());
		default:
			MCTHROW("Invalid STANDARD_PSO value.");
		}
	}

#pragma endregion

}