#pragma once

#include "MCD3D12Core.h"

#include "RenderConfig.h"
#include "MCDXGI.h"
#include "MCUploadBuffer.h"
#include "MCMath.h"
#include "../../../../Common/MCCommon/src/Headers/MCFrame.h"

#include "MCMesh.h"

#include "../Shader/MCShaderManager.h"
#include <atomic>

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCVertex1Color {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct ObjectConstants {
		DirectX::XMFLOAT4X4 WorldViewProj = MCMath::Identity4x4();
		float  time                      = 1.0;
	};

	class MCD3D {
	public:
		/*
			MC3D is a singleton. Delete all copy and assignment methods.
		*/
		MCD3D(MCD3D&)              = delete;
		MCD3D(MCD3D&&)             = delete;
		MCD3D& operator= (MCD3D&)  = delete;
		MCD3D& operator= (MCD3D&&) = delete;

		/*
			Simple destructor. MCD3D does not own any heap objects.
		*/
		~MCD3D();

		/*
			Access to the MCD3D singleton instance.
		*/
		inline static MCD3D *Instance() {
			static MCD3D instance;
			return &instance;
		}

	private:
		/*
			MCD3D is a singleton. The constructor is private.
		*/
		MCD3D();

	public:
		/*  Initializes the MCD3D instance. This method associates an initial configuration with the instance
			and initializes the instance. This method must be the first method call to the instance. */
		void Initialize(const RENDER_CONFIG& renderConfig);

		/*	Return false, until the Initialize method has been called. */ 
		bool Initialized() { return _initialized; }

		/*	Get the current fence value. */
		unsigned __int64 GetCurrentFenceValue() const;

		/*	Create a new fence and return it. */
		unsigned __int64 GetNewFenceValue();

		/*	Block until the specified fence is reached. */
		void WaitForFenceValue(unsigned __int64 fenceValue) const;

	public:
		/* Execute the command list. */
		void ExecuteCommandList(ID3D12CommandList *pCommandList) const;

		/* Execute the command lists. */
		void ExecuteCommandLists(int numCommandLists, ID3D12CommandList* const *pCommandLists) const;

		void RenderFrame(const MCFrame *pFrame);

		void QuickDraw();
		void FlushCommandQueue();

	public:
		void OrderedStaticDestroy();

	private:

		ComPtr<ID3D12Fence>               _pFence;
		std::atomic_int64_t               _currentFence;

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

		//// Initialize the views to the box geometry.
		//void InitBoxGeometryViews();

		// Create the root signature.
		void InitBoxRootSignature();

		// Load the vertex and pixel shaders.
		void InitShaders();

		// Init the pipeline state for the box
		void InitBoxPSO();

		void TestUpdate(const MCFrame* pFrame);

		std::unique_ptr<MCStaticMesh16<MCVertex1Color>>              _pSphereMesh;
		std::unique_ptr<MCStaticAllocatedMesh16<MC::MCVertex1Color>> _pCubeMesh;

		ComPtr<ID3D12RootSignature> _pBoxRootSignature;
		D3D12_INPUT_ELEMENT_DESC    _pElementLayoutDescriptions[2];
		ComPtr<ID3D12PipelineState> _pBoxPSO;

		//ComPtr<ID3DBlob>            _pBoxVertexShader;
		//ComPtr<ID3DBlob>            _pBoxPixelShader;

		MCShaderHandle	_hVertexShader;
		MCShaderHandle  _hPixelShader;

	private:

		/*
			Call to resize the swap chain buffers. Will release and restore resources.
		*/
		void Resize();

	private:
		const RENDER_CONFIG _initialConfiguration;

		// The device is actually created and owned by the MCDXGI. Specifically
		// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the MCDXGI
		// goes out of scope, this pointer will be destroyed.
		ID3D12Device *_pDevice;

		// Set to true at the end of the Initialize method.
		bool _initialized;
	};
	
	class MCD3D12RenderEngine {
	public:
		~MCD3D12RenderEngine();
		MCD3D12RenderEngine(MCD3D12RenderEngine&) = delete;
		MCD3D12RenderEngine(MCD3D12RenderEngine&&) = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&) = delete;
		MCD3D12RenderEnging& operator=(MCD3D12RenderEngine&&) = delete;
	private:
		MCD3D12RenderEngine();
	public:
		static MCD3D12RenderEngine *pInstance;
		static MCD3D               *pMCD3D;
		static MCDXGI              *pMCDXGI;
		static MCResourceManager   *pResourceManager;
		static MCShaderManager     *pShaderManager;
	private:
		std::unique_ptr<MCD3D>             _pMCD3D;
		std::unique_ptr<MCDXGI>            _pDXGI;
		std::unique_ptr<MCResourceManager> _pResourceManager;
		std::unique_ptr<MCShaderManager>   _pShaderManager;
	}
}
