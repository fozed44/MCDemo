#pragma once

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

		InitDevice(pDXGIWrapper);

		MC_INFO("End render initialization.");
	}

	void D3DWrapper::InitDevice(std::shared_ptr<DXGIWrapper>& pDXGIWrapper) {
		// TODO:
		//	We don't really want to be messing with raw com pointers that need to be released.
		//  FIND AND USE A BETTER SOLUTION.
		auto pAdapter = pDXGIWrapper->GetConfiguredOrDefaultAdapter();

		INIT_INFO("Creating 3D Device.");

		MCThrowIfFailed(D3D12CreateDevice(
			(IUnknown*)pAdapter,
			D3D_FEATURE_LEVEL_11_0,
			__uuidof(_pDevice),
			&_pDevice
		));

		INIT_INFO("Device successfully created.");

		MCSAFE_RELEASE(pAdapter);
	}
	void D3DWrapper::InitFence() {

	}
	void D3DWrapper::InitCommandQueue() {

	}
	void D3DWrapper::InitCommandAllocator() {

	}
	void D3DWrapper::InitCommandList() {

	}

#pragma endregion 
}
