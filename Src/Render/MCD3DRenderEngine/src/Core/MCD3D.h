#pragma once

#include "MCD3D12Core.h"

#include "MCDXGI.h"
#include "MCUploadBuffer.h"
#include "MCMath.h"
#include "../../../../Common/MCCommon/src/Data/MCFrame.h"

#include "../Mesh/MCMesh.h"

#include "../Shader/MCShaderManager.h"
#include <atomic>

using Microsoft::WRL::ComPtr;

#define MC_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace MC {

	

#pragma region old

	//class MCD3D {
	//public:
	//	MCD3D();
	//	~MCD3D();
	//	MCD3D(MCD3D&)              = delete;
	//	MCD3D(MCD3D&&)             = delete;
	//	MCD3D& operator= (MCD3D&)  = delete;
	//	MCD3D& operator= (MCD3D&&) = delete;

	//public:
	//	/*	Get the current fence value. */
	//	unsigned __int64 GetCurrentFenceValue() const;

	//	/*	Create a new fence and return it. */
	//	unsigned __int64 GetNewFenceValue();

	//	/*	Block until the specified fence is reached. */
	//	void WaitForFenceValue(unsigned __int64 fenceValue) const;

	//public:
	//	/* Execute the command list. */
	//	void ExecuteCommandList(ID3D12CommandList *pCommandList) const;

	//	/* Execute the command lists. */
	//	void ExecuteCommandLists(int numCommandLists, ID3D12CommandList* const *pCommandLists) const;

	//	void RenderFrame(const MCFrame3D *pFrame);

	//	void QuickDraw();
	//	void FlushCommandQueue();

	//public:
	//	void OrderedStaticDestroy();

	//private:

	//	ComPtr<ID3D12Fence>               _pFence;
	//	std::atomic_int64_t               _currentFence;

	//	ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;
	//	ComPtr<ID3D12CommandQueue>        _pCommandQueue;
	//	ComPtr<ID3D12GraphicsCommandList> _pCommandList;

	//	// Render target view descriptor heap.
	//	ComPtr<ID3D12DescriptorHeap>      _pRTVDescriptorHeap;
	//	UINT                              _RTVDescriptorSize;

	//	// Constant buffer view descriptor heap.
	//	ComPtr<ID3D12DescriptorHeap>      _pCBVDescriptorHeap;
	//	UINT                              _CBVDescriptorSize;

	//	// Depth/Stencil buffer view descriptor heap.
	//	ComPtr<ID3D12DescriptorHeap>      _pDSVDescriptorHeap;
	//	UINT                              _DSVDescriptorSize;

	//	// Render Targets
	//	ComPtr<ID3D12Resource>            _ppRenderTargets[FRAME_BUFFER_COUNT];

	//	// Depth Stencil
	//	ComPtr<ID3D12Resource>            _pDepthStencil;

	//	// Buffer to hold the cubes matrix visible to the pipe-line
	//	std::unique_ptr<MCUploadBuffer<ObjectConstants>> _pObjectConstantBuffer;

	//	// Matrices
	//	DirectX::XMFLOAT4X4               _worldMatrix;
	//	DirectX::XMFLOAT4X4               _viewMatrix;
	//	DirectX::XMFLOAT4X4               _projectionMatrix;

	//	D3D12_VIEWPORT                    _viewPort;
	//	D3D12_RECT                        _scissorRect;

	//private:
	//	void Init3DDevice();
	//	void InitFence();
	//	void InitCommandQueue();
	//	void InitCommandAllocator();
	//	void InitCommandList();
	//	void InitSwapChain();
	//	void InitDescriptorHeaps();
	//	void InitRenderTargets();
	//	void InitRenderTargetViews();
	//	void InitDepthStencilBuffer();
	//	void InitDepthStencilBufferView();
	//	void InitConstantBuffer();
	//	void InitConstantBufferView();
	//	void InitViewPort();
	//	void InitFinalize();
	//	void InitMatrices();

	//	void EnsureValidWindowConfig(const RENDER_CONFIG& renderConfig)const;

	//	// Test methods that may be deleted
	//	////////////////////////////////////////////////////////////

	//public:
	//	/// Main Test init that is called by Init();
	//	void InitTest();

	//private:
	//	// Get test data into _boxVerts and _boxIndicies.
	//	void InitBoxGeometry();

	//	//// Initialize the views to the box geometry.
	//	//void InitBoxGeometryViews();

	//	// Create the root signature.
	//	void InitBoxRootSignature();

	//	// Load the vertex and pixel shaders.
	//	void InitShaders();

	//	// Init the pipeline state for the box
	//	void InitBoxPSO();

	//	void TestUpdate(const MCFrame3D* pFrame);

	//	std::unique_ptr<MCStaticMesh16<MCVertex1Color>>              _pSphereMesh;
	//	std::unique_ptr<MCStaticAllocatedMesh16<MC::MCVertex1Color>> _pCubeMesh;

	//	ComPtr<ID3D12RootSignature> _pBoxRootSignature;
	//	D3D12_INPUT_ELEMENT_DESC    _pElementLayoutDescriptions[2];
	//	ComPtr<ID3D12PipelineState> _pBoxPSO;

	//	//ComPtr<ID3DBlob>            _pBoxVertexShader;
	//	//ComPtr<ID3DBlob>            _pBoxPixelShader;

	//	MCShaderHandle	_hVertexShader;
	//	MCShaderHandle  _hPixelShader;

	//private:

	//	/*
	//		Call to resize the swap chain buffers. Will release and restore resources.
	//	*/
	//	void Resize();

	//private:
	//	// The device is actually created and owned by the MCDXGI. Specifically
	//	// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the MCDXGI
	//	// goes out of scope, this pointer will be destroyed.
	//	ID3D12Device *_pDevice;

	//};	

#pragma endregion

	class MCD3D {
	public:
		MCD3D();
		~MCD3D();
		MCD3D(MCD3D&)              = delete;
		MCD3D(MCD3D&&)             = delete;
		MCD3D& operator= (MCD3D&)  = delete;
		MCD3D& operator= (MCD3D&&) = delete;

	public: /* Fencing */

		/*	Get the current fence value. */
		unsigned __int64 GetCurrentFenceValue() const;

		/*	Create a new fence and return it. */
		unsigned __int64 GetNewFenceValue();

		/*	Block until the specified fence is reached. */
		void WaitForFenceValue(unsigned __int64 fenceValue) const;

		/* Flush the local command queue. */
		void FlushCommandQueue();

	public:
		/* Execute the command list. */
		void ExecuteCommandList(ID3D12CommandList *pCommandList) const;

		/* Execute the command lists. */
		void ExecuteCommandLists(int numCommandLists, ID3D12CommandList* const *pCommandLists) const;

	public: /* RenderTarget / DepthStencil access */
		/* Get a pointer to a specific render target. */
		ID3D12Resource* GetRenderTarget(unsigned int index);

		/* Get a descriptor to a specific render target. */
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCPUHandle(unsigned int index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTargetGPUHandle(unsigned int index);

		/* Get a pointer to the depth stencil. */
		ID3D12Resource* GetDepthStencil();

		/* Get a CPU descriptor to the depth stencil. */
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilCPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetDepthStencilGPUHandle();

	private:

		ComPtr<ID3D12Fence>               _pFence;
		std::atomic_int64_t               _currentFence;

		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;
		ComPtr<ID3D12CommandQueue>        _pCommandQueue;
		ComPtr<ID3D12GraphicsCommandList> _pCommandList;

		// Render target view descriptor heap.
		ComPtr<ID3D12DescriptorHeap>      _pRTVDescriptorHeap;
		UINT                              _RTVDescriptorSize;

		// Depth/Stencil buffer view descriptor heap.
		ComPtr<ID3D12DescriptorHeap>      _pDSVDescriptorHeap;
		UINT                              _DSVDescriptorSize;

		// Render Targets
		static_assert(FRAME_BUFFER_COUNT == 2, "This engine must have 2 buffers, a front buffer and a back buffer.");
		ComPtr<ID3D12Resource>            _ppRenderTargets[FRAME_BUFFER_COUNT];

		// Depth Stencil
		ComPtr<ID3D12Resource>            _pDepthStencil;

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
		void InitDepthStencilBuffers();
		void InitDepthStencilBufferViews();
		void InitFinalize();

	private:

		/*
		Call to resize the swap chain buffers. Will release and restore resources.
		*/
		void Resize();

	private:
		// The device is actually created and owned by the MCDXGI. Specifically
		// calling GXGIWrapper.CreateConfiguredOrDefault3DDevice or if the MCDXGI
		// goes out of scope, this pointer will be destroyed.
		ID3D12Device *_pDevice;

	};

}
