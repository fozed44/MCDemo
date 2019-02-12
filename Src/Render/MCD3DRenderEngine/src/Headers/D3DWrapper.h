#pragma once

//#include "dxgi1_6.h"
#include "d3d12.h"
#include "DirectxMath.h"
#include "wrl.h"
#include <memory>

#include "MCD3DRenderEngineConstants.h"
#include "RenderConfig.h"
#include "DXGIWrapper.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCVertex1Color {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	class D3DWrapper {
	public:
		D3DWrapper(const RENDER_CONFIG& renderConfig);
		~D3DWrapper();

	public:
		void Init(std::shared_ptr<DXGIWrapper>& pDXGIWrapper);

		void QuickDraw();
		void FlushCommandQueue();


	private:

		ComPtr<ID3D12Fence>               _pFence;
		UINT64                            _currentFence;

		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;
		ComPtr<ID3D12CommandQueue>        _pCommandQueue;
		ComPtr<ID3D12GraphicsCommandList> _pCommandList;

		// Render target view descriptor heap.
		ComPtr<ID3D12DescriptorHeap>      _pRTVDescriptorHeap;
		UINT                              _RTVDescriptorSize;

		// Constant buffer view descriptor heap.
		ComPtr<ID3D12DescriptorHeap>      _pCBVDescriptorHeap;
		UINT                              _CBVDescriptorSize;

		// Depth/Stencil buffer view descriptor heap.
		ComPtr<ID3D12DescriptorHeap>      _pDSVDescriptorHeap;
		UINT                              _DSVDescriptorSize;

		// Render Targets
		ComPtr<ID3D12Resource>            _ppRenderTargets[FRAME_BUFFER_COUNT];

		// Depth Stencil
		ComPtr<ID3D12Resource>            _pDepthStencil;

		D3D12_VIEWPORT                    _viewPort;

	private:
		void Init3DDevice();
		void InitFence();
		void InitCommandQueue();
		void InitCommandAllocator();
		void InitCommandList();
		void InitSwapChain();
		void InitDescriptorHeaps();
		void InitRenderTargets();
		void InitRTVHeap();
		void InitDepthStencilBuffer();
		void InitDepthStencilBufferView();
		void InitViewPort();
		void InitFinalize();

		void EnsureValidWindowConfig();

	private:

		ComPtr<ID3D12Resource> CreateDefaultBuffer(void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);

	private:
		const RENDER_CONFIG _initialConfiguration;

		std::shared_ptr<DXGIWrapper> _pDXGIWrapper;

		// The device is actually created and owned by the DXGIWrapper. Specifically
		// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the DXGIWrapper
		// goes out of scope, this pointer will be destroyed.
		ID3D12Device *_pDevice;
	};
}