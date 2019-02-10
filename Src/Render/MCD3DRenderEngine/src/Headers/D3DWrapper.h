#pragma once

//#include "dxgi1_6.h"
#include "d3d12.h"
#include "wrl.h"
#include <memory>

#include "MCD3DRenderEngine.h"
#include "RenderConfig.h"
#include "DXGIWrapper.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class D3DWrapper {
	public:
		D3DWrapper();
		~D3DWrapper();

	public:
		void Init(const RENDER_CONFIG* pRenderConfig, std::shared_ptr<DXGIWrapper>& pDXGIWrapper);

	private:

		ComPtr<ID3D12Fence>               _pFence;
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

		// Render Target Views
		ComPtr<ID3D12Resource>            _ppRenderTargets[FRAME_BUFFER_COUNT];

		// Depth Stencil View
		ComPtr<ID3D12Resource>            _pDepthStencilView;

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
		void InitDepthStencilBufferView();

	private:
		std::shared_ptr<DXGIWrapper> _pDXGIWrapper;

		// The device is actually created and owned by the DXGIWrapper. Specifically
		// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the DXGIWrapper
		// goes out of scope, this pointer will be destroyed.
		ID3D12Device *_pDevice;
	};
}