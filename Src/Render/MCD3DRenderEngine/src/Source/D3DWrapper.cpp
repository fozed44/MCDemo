#pragma once

#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../Headers/d3dx12.h"

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

		// Store a pointer to the dxgi wrapper.
		_pDXGIWrapper = pDXGIWrapper;
		
		Init3DDevice();
		InitFence();
		InitCommandQueue();
		InitCommandAllocator();
		InitCommandList();
		InitSwapChain();
		InitDescriptorHeaps();
		InitRenderTargets();
		InitRTVHeap();
		InitDepthStencilBufferView();

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

	void D3DWrapper::InitSwapChain() {
		INIT_TRACE("Begin swapchain init.");

		_pDXGIWrapper->CreateConfiguredOrDefaltSwapchain(_pCommandQueue.Get());

		INIT_TRACE("End swapchain init.");
	}

	void D3DWrapper::Init3DDevice() {
		INIT_TRACE("Begin 3d device init.");

		_pDevice = _pDXGIWrapper->CreateConfiguredOrDefault3DDevice();

		INIT_TRACE("End 3d device init.");
	}

	void D3DWrapper::InitDescriptorHeaps() {
		INIT_TRACE("Begin descriptor heap init.");

		// Render Target Descriptor Heap ****************************

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

		rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
		rtvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(_pRTVDescriptorHeap), &_pRTVDescriptorHeap));

		// Constant Buffer Descriptor Heap ****************************

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

		cbvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
		cbvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&cbvHeapDesc, __uuidof(_pCBVDescriptorHeap), &_pCBVDescriptorHeap));

		// Depth Stencil Buffer Descriptor Heap *************************

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

		dsvHeapDesc.NumDescriptors = 1; // One single depth stencil buffer.
		dsvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(_pDSVDescriptorHeap), &_pDSVDescriptorHeap));
		
		_RTVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_CBVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_DSVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		INIT_TRACE("End descriptor heap init.");
	}

	void D3DWrapper::InitRenderTargets() {
		INIT_TRACE("Begin init render target view.");

		// Create a render target view for each swap chain buffer
		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
			_pDXGIWrapper->GetFrameBuffer(n, __uuidof(_ppRenderTargets[n]), &_ppRenderTargets[n]);
		}

		INIT_TRACE("End init render target view.");
	}

	void D3DWrapper::InitRTVHeap() {
		INIT_TRACE("Begin init RTV Heap.");

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
			_pDevice->CreateRenderTargetView(_ppRenderTargets[n].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, _RTVDescriptorSize);
		}

		INIT_TRACE("End init RTV Heap.");
	}

	void D3DWrapper::InitDepthStencilBufferView() {
		INIT_TRACE("Begin depth stencil buffer view.");

		INIT_TRACE("End depth stencil buffer view.");
	}

#pragma endregion 
}
