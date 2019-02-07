#pragma once

#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../Headers/D3DWrapper.h"
#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

namespace MC {

#pragma region CtorDtor

	D3DWrapper::D3DWrapper() {}

	D3DWrapper::~D3DWrapper() {}

#pragma endregion 

#pragma region Initialization

	void D3DWrapper::Init(const RENDER_CONFIG *pRenderConfig, std::shared_ptr<DXGIWrapper>& pDXGIWrapper) {
		MC_INFO("Begin render initialization.");

		assert(pDXGIWrapper->Initialized());

		_pDevice = pDXGIWrapper->CreateConfiguredOrDefault3DDevice();

		InitFence();
		InitCommandQueue();
		InitCommandAllocator();
		InitCommandList();


		MC_INFO("End render initialization.");
	}

	void D3DWrapper::InitFence() {
		INIT_TRACE("Begin d3d12 fence init.");
		MCThrowIfFailed(_pDevice->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			__uuidof(_pFence),
			&_pFence
		));
		INIT_TRACE("End d3d12 fence init.");
	}
	void D3DWrapper::InitCommandQueue() {
		INIT_TRACE("Begin d3d12 command queue init.");

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		MCThrowIfFailed(_pDevice->CreateCommandQueue(
			&queueDesc,
			__uuidof(_pCommandQueue),
			&_pCommandQueue
		));

		INIT_TRACE("End d3d12 command queue init.");
	}
	void D3DWrapper::InitCommandAllocator() {
		INIT_TRACE("Begin d3d12 command allocator queue init.");

		MCThrowIfFailed(_pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			__uuidof(_pCommandAllocator),
			&_pCommandAllocator
		));

		INIT_TRACE("End d3d12 command allocator queue inti.");
	}

	void D3DWrapper::InitCommandList() {
		INIT_TRACE("Begin d3d12 command list init.");

		MCThrowIfFailed(_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			_pCommandAllocator.Get(),
			nullptr,
			__uuidof(_pCommandList),
			&_pCommandList
		));

		// The command queue should begin in a closed state because the render loop
		// will expect it to be in a closed state.
		_pCommandList->Close();

		INIT_TRACE("End d3d12 command list init.");
	}

#pragma endregion 
}
