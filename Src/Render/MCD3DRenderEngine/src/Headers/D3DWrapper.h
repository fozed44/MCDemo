#pragma once

#include "MCD3D12Core.h"

#include "RenderConfig.h"
#include "DXGIWrapper.h"
#include "MCUploadBuffer.h"
#include "MCMath.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCVertex1Color {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct ObjectConstants {
		DirectX::XMFLOAT4X4 WorldViewProj = MCMath::Identity4x4();
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

		// Buffer to hold the cubes matrix visible to the pipe-line
		std::unique_ptr<MCUploadBuffer<ObjectConstants>> _pObjectConstantBuffer;

		// Matrices
		DirectX::XMFLOAT4X4               _worldMatrix;
		DirectX::XMFLOAT4X4               _viewMatrix;
		DirectX::XMFLOAT4X4               _projectionMatrix;

		D3D12_VIEWPORT                    _viewPort;
		D3D12_RECT                        _scissorRect;

	private:
		void Init3DDevice();
		void InitFence();
		void InitCommandQueue();
		void InitCommandAllocator();
		void InitCommandList();
		void InitSwapChain();
		void InitDescriptorHeaps();
		void InitRenderTargets();
		void InitRenderTargetViews();
		void InitDepthStencilBuffer();
		void InitDepthStencilBufferView();
		void InitConstantBuffer();
		void InitConstantBufferView();
		void InitViewPort();
		void InitFinalize();
		void InitMatrices();

		void EnsureValidWindowConfig();

		// Test methods that may be deleted
		////////////////////////////////////////////////////////////

		/// Main Test init that is called by Init();
		void InitTest();

		// Get test data into _boxVerts and _boxIndicies.
		void InitBoxGeometry();

		// Initialize the views to the box geometry.
		void InitBoxGeometryViews();

		// Create the root signature.
		void InitBoxRootSignature();

		// Load the vertex and pixel shaders.
		void InitShaders();

		// Init the pipeline state for the box
		void InitBoxPSO();

		void TestUpdate();

		// execute GPU commands synchronously
		void ExecSync(void (*func)());

		ComPtr<ID3D12Resource>      _pBoxVerts;
		ComPtr<ID3D12Resource>      _pBoxIndicies;

		D3D12_VERTEX_BUFFER_VIEW    _boxVertView;
		D3D12_INDEX_BUFFER_VIEW     _boxIndexView;

		ComPtr<ID3D12RootSignature> _pBoxRootSignature;
		D3D12_INPUT_ELEMENT_DESC    _pElementLayoutDescriptions[2];
		ComPtr<ID3D12PipelineState> _pBoxPSO;

		ComPtr<ID3DBlob>            _pBoxVertexShader;
		ComPtr<ID3DBlob>            _pBoxPixelShader;

	private:

		ComPtr<ID3D12Resource> CreateDefaultBuffer(void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);

		ComPtr<ID3DBlob> LoadShader(const std::string& filename) const;

	private:
		const RENDER_CONFIG _initialConfiguration;

		std::shared_ptr<DXGIWrapper> _pDXGIWrapper;

		// The device is actually created and owned by the DXGIWrapper. Specifically
		// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the DXGIWrapper
		// goes out of scope, this pointer will be destroyed.
		ID3D12Device *_pDevice;
	};
}