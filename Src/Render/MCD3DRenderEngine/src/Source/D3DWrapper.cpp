#pragma once

#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../Headers/d3dx12.h"

#include "../Headers/D3DWrapper.h"
#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

#define MC_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace MC {

#pragma region CtorDtor

	D3DWrapper::D3DWrapper(const RENDER_CONFIG& renderConfig) 
		: _initialConfiguration(renderConfig) {}

	D3DWrapper::~D3DWrapper() {}

#pragma endregion 

#pragma region Initialization

	void D3DWrapper::Init(std::shared_ptr<DXGIWrapper>& pDXGIWrapper) {
		MC_INFO("Begin render initialization.");

		assert(pDXGIWrapper->Initialized());

		EnsureValidWindowConfig();

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
		InitDepthStencilBuffer();
		InitDepthStencilBufferView();
		InitViewPort();

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

	void D3DWrapper::InitDepthStencilBuffer() {
		INIT_TRACE("Init depth stencil buffer.");

		D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
		depthStencilBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilBufferDesc.Alignment = 0;
		depthStencilBufferDesc.Width = _initialConfiguration.DISPLAY_OUTPUT_WIDTH;
		depthStencilBufferDesc.Height = _initialConfiguration.DISPLAY_OUTPUT_HEIGHT;
		depthStencilBufferDesc.DepthOrArraySize = 1;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.Format = MC_DEPTH_STENCIL_FORMAT;

		// TODO:
		//		OPTIONS_MULTISAMPLE need to be verified since it comes from the config file.
		depthStencilBufferDesc.SampleDesc.Count = _initialConfiguration.OPTIONS_MULTISAMPLE == 1 ? 4 : 1;

		// TODO
		//	Quality is related to a value that needs to be added to the configuration... We need a OPTIONS_MULTISAMPLE_QUALITY
		depthStencilBufferDesc.SampleDesc.Quality = 0;

		depthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		
		optClear.Format = MC_DEPTH_STENCIL_FORMAT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		MCThrowIfFailed(_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,	
			&depthStencilBufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			__uuidof(_pDepthStencil),
			&_pDepthStencil
		));

		INIT_TRACE("End init depth stencil buffer.");
	}

	void D3DWrapper::InitDepthStencilBufferView() {
		INIT_TRACE("Begin init depth stencil buffer view.");

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// TODO:
		//	Unsure about dsvHeapHandle
		_pDevice->CreateDepthStencilView(
			_pDepthStencil.Get(),
			nullptr,
			dsvHeapHandle
		);

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(_pDepthStencil.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			)
		);

		INIT_TRACE("End init depth stencil buffer view.");
	}

	void D3DWrapper::InitViewPort() {
		INIT_TRACE("Begin init view port.");

		D3D12_VIEWPORT vp = {};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width    = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_WIDTH);
		vp.Height   = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_HEIGHT);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		_pCommandList->RSSetViewports(1, &vp);

		INIT_TRACE("End view port.");
	}


	/*
		Examine the window width and height in _initialConfiguration. Throw an exception if the values do not
		fall within an acceptable range.
	*/
	void D3DWrapper::EnsureValidWindowConfig() {
		if (_initialConfiguration.DISPLAY_OUTPUT_WIDTH <= 0
			|| _initialConfiguration.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_WIDTH) {
			INIT_ERROR("Detected an invalid window width ({0:d}) in the config file.", _initialConfiguration.DISPLAY_OUTPUT_WIDTH);
			MCTHROW("Detected an invalid window width in the config file.");
		}

		if (_initialConfiguration.DISPLAY_OUTPUT_HEIGHT <= 0
			|| _initialConfiguration.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_HEIGHT) {
			INIT_ERROR("Detected an invalid window height ({0:d}) in the config file.", _initialConfiguration.DISPLAY_OUTPUT_HEIGHT);
			MCTHROW("Detected an invalid window height in the config file.");
		}
	}

#pragma endregion 
}
