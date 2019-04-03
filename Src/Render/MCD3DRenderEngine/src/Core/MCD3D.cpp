#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "d3dx12.h"

#include "MCD3D.h"
#include "../Configuration/RenderConfig.h"
#include "MCREGlobals.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"

#include "TestFunctions.h"
#include <fstream>
#include <string>


#include "MCD3D12RenderUtilities.h"

#define MC_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace MC {

#pragma region old

//#pragma region CtorDtor
//
//	MCD3D::MCD3D() 
//		: _pObjectConstantBuffer(nullptr),
//		  _pElementLayoutDescriptions{} {
//		MC_INFO("Begin render initialization.");
//
//		MC_INFO("atomic fence is lock free: {0:d}", (int)_currentFence.is_lock_free());
//
//		assert(MCREGlobals::pMCDXGI);
//
//
//		EnsureValidWindowConfig(MCREGlobals::InitialConfiguration);
//
//		// initialize the current fence value
//		_currentFence = 0;
//
//		Init3DDevice();
//		InitFence();
//		InitCommandQueue();
//		InitCommandAllocator();
//		InitCommandList();
//		InitSwapChain();
//		InitDescriptorHeaps();
//		InitRenderTargets();
//		InitRenderTargetViews();
//		InitDepthStencilBuffer();
//		InitDepthStencilBufferView();
//		InitConstantBuffer();
//		InitConstantBufferView();
//		InitViewPort();
//		InitMatrices();
//		InitFinalize();
//
//		MC_INFO("End render initialization.");
//	}
//
//	MCD3D::~MCD3D() {}
//
//#pragma endregion 
//
//#pragma region Initialization
//
//	void MCD3D::InitFence() {
//		INIT_TRACE("Begin d3d12 fence init.");
//		MCThrowIfFailed(_pDevice->CreateFence(
//			0,
//			D3D12_FENCE_FLAG_NONE,
//			__uuidof(_pFence),
//			&_pFence
//		));
//		_pFence->SetName(L"MCD3D fence.");
//		INIT_TRACE("End d3d12 fence init.");
//	}
//
//	void MCD3D::InitCommandQueue() {
//		INIT_TRACE("Begin d3d12 command queue init.");
//
//		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//		queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
//		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//
//		MCThrowIfFailed(_pDevice->CreateCommandQueue(
//			&queueDesc,
//			__uuidof(_pCommandQueue),
//			&_pCommandQueue
//		));
//
//		_pCommandQueue->SetName(L"Command Queue");
//
//		INIT_TRACE("End d3d12 command queue init.");
//	}
//
//	void MCD3D::InitCommandAllocator() {
//		INIT_TRACE("Begin d3d12 command allocator queue init.");
//
//		MCThrowIfFailed(_pDevice->CreateCommandAllocator(
//			D3D12_COMMAND_LIST_TYPE_DIRECT,
//			__uuidof(_pCommandAllocator),
//			&_pCommandAllocator
//		));
//
//		_pCommandAllocator->SetName(L"Command Allocator");
//
//		INIT_TRACE("End d3d12 command allocator queue inti.");
//	}
//
//	void MCD3D::InitCommandList() {
//		INIT_TRACE("Begin d3d12 command list init.");
//
//		MCThrowIfFailed(_pDevice->CreateCommandList(
//			0,
//			D3D12_COMMAND_LIST_TYPE_DIRECT,
//			_pCommandAllocator.Get(),
//			nullptr,
//			__uuidof(_pCommandList),
//			&_pCommandList
//		));
//
//		_pCommandList->SetName(L"Default command list");
//
//		// The command queue should begin in a closed state because the render loop
//		// will expect it to be in a closed state.
//		_pCommandList->Close();
//
//		INIT_TRACE("End d3d12 command list init.");
//	}
//
//	void MCD3D::InitSwapChain() {
//		INIT_TRACE("Begin swapchain init.");
//
//		MCREGlobals::pMCDXGI->CreateConfiguredOrDefaltSwapchain(_pCommandQueue.Get());
//
//		INIT_TRACE("End swapchain init.");
//	}
//
//	void MCD3D::Init3DDevice() {
//		INIT_TRACE("Begin 3d device init.");
//
//		_pDevice = MCREGlobals::pMCDXGI->CreateConfiguredOrDefault3DDevice();
//
//		INIT_TRACE("End 3d device init.");
//	}
//
//	void MCD3D::InitDescriptorHeaps() {
//		INIT_TRACE("Begin descriptor heap init.");
//
//		// Render Target Descriptor Heap ****************************
//
//		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//
//		rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
//		rtvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(_pRTVDescriptorHeap), &_pRTVDescriptorHeap));
//
//		// Constant Buffer Descriptor Heap ****************************
//
//		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
//
//		cbvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
//		cbvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//		cbvHeapDesc.NodeMask = 0;
//		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&cbvHeapDesc, __uuidof(_pCBVDescriptorHeap), &_pCBVDescriptorHeap));
//
//		// Depth Stencil Buffer Descriptor Heap *************************
//
//		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
//
//		dsvHeapDesc.NumDescriptors = 1; // One single depth stencil buffer.
//		dsvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//		MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(_pDSVDescriptorHeap), &_pDSVDescriptorHeap));
//		
//		_RTVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		_CBVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//		_DSVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//
//		INIT_TRACE("End descriptor heap init.");
//	}
//
//	void MCD3D::InitRenderTargets() {
//		INIT_TRACE("Begin init render targets.");
//
//		// Create a render target view for each swap chain buffer
//		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
//			MCREGlobals::pMCDXGI->GetFrameBuffer(n, __uuidof(_ppRenderTargets[n]), &_ppRenderTargets[n]);
//		}
//
//		INIT_TRACE("End init render targets.");
//	}
//
//	void MCD3D::InitRenderTargetViews() {
//		INIT_TRACE("Begin init RTV Heap.");
//
//		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
//
//		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
//			_pDevice->CreateRenderTargetView(_ppRenderTargets[n].Get(), nullptr, rtvHeapHandle);
//			rtvHeapHandle.Offset(1, _RTVDescriptorSize);
//		}
//
//		INIT_TRACE("End init RTV Heap.");
//	}
//
//	void MCD3D::InitDepthStencilBuffer() {
//		INIT_TRACE("Init depth stencil buffer.");
//
//		int width, height;
//
//		MCREGlobals::pMCDXGI->GetFrameBufferSize(&width, &height);
//
//		D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
//		depthStencilBufferDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//		depthStencilBufferDesc.Alignment        = 0;
//		depthStencilBufferDesc.Width            = width;
//		depthStencilBufferDesc.Height           = height;
//		depthStencilBufferDesc.DepthOrArraySize = 1;
//		depthStencilBufferDesc.MipLevels        = 1;
//		depthStencilBufferDesc.Format           = MC_DEPTH_STENCIL_FORMAT;
//		depthStencilBufferDesc.SampleDesc.Count = 1;
//
//		depthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//		depthStencilBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//		D3D12_CLEAR_VALUE optClear;
//		
//		optClear.Format = MC_DEPTH_STENCIL_FORMAT;
//		optClear.DepthStencil.Depth = 1.0f;
//		optClear.DepthStencil.Stencil = 0;
//
//		MCThrowIfFailed(_pDevice->CreateCommittedResource(
//			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//			D3D12_HEAP_FLAG_NONE,	
//			&depthStencilBufferDesc,
//			D3D12_RESOURCE_STATE_COMMON,
//			&optClear,
//			__uuidof(_pDepthStencil),
//			&_pDepthStencil
//		));
//
//		_pDepthStencil->SetName(L"Depth stencil");
//
//		INIT_TRACE("End init depth stencil buffer.");
//	}
//
//	void MCD3D::InitDepthStencilBufferView() {
//		INIT_TRACE("Begin init depth stencil buffer view.");
//
//		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
//
//		// TODO:
//		//	Unsure about dsvHeapHandle
//		_pDevice->CreateDepthStencilView(
//			_pDepthStencil.Get(),
//			nullptr,
//			dsvHeapHandle
//		);		
//
//		INIT_TRACE("End init depth stencil buffer view.");
//	}
//
//	void MCD3D::InitConstantBuffer() {
//		INIT_TRACE("Init constant buffer.");
//
//		_pObjectConstantBuffer = std::make_unique<MCUploadBuffer<ObjectConstants>>(_pDevice, 1, true);
//
//		INIT_TRACE("End init constant buffer.");
//	}
//
//	void MCD3D::InitConstantBufferView() {
//		INIT_TRACE("Init constant buffer view");
//
//		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = _pObjectConstantBuffer->Resource()->GetGPUVirtualAddress();
//
//		// For the moment we are just getting a view for the box.
//		unsigned int boxConstantBufferCalculatedSize = _pObjectConstantBuffer->CalculatedSize();
//		unsigned int boxConstantBufferIndex = 0;
//		cbAddress += boxConstantBufferIndex * boxConstantBufferCalculatedSize;
//
//		D3D12_CONSTANT_BUFFER_VIEW_DESC boxConstantBufferViewDesc = {};
//		boxConstantBufferViewDesc.BufferLocation = cbAddress;
//		boxConstantBufferViewDesc.SizeInBytes = boxConstantBufferCalculatedSize;
//
//		_pDevice->CreateConstantBufferView(
//			&boxConstantBufferViewDesc,
//			_pCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
//		);
//
//		INIT_TRACE("End init constant buffer view.");
//	}
//
//	void MCD3D::InitViewPort() {
//		INIT_TRACE("Begin init view port.");
//
//		auto width = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_WIDTH;
//		auto height = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_HEIGHT;
//
//		_viewPort = {};
//		_viewPort.TopLeftX = 0.0f;
//		_viewPort.TopLeftY = 0.0f;
//		_viewPort.Width    = static_cast<float>(width);
//		_viewPort.Height   = static_cast<float>(height);
//		_viewPort.MinDepth = 0.0f;
//		_viewPort.MaxDepth = 1.0f;
//
//		_scissorRect.top    = 0;
//		_scissorRect.left   = 0;
//		_scissorRect.right  = width;
//		_scissorRect.bottom = height;
//		/*_pCommandList->RSSetViewports(1, &_viewPort);*/
//
//		INIT_TRACE("End view port.");
//	}
//
//	void MCD3D::InitFinalize() {
//		INIT_TRACE("Begin init finalize.");
//
//		INIT_TRACE("--Reset command allocator.");
//		MCThrowIfFailed(_pCommandAllocator->Reset());
//
//		INIT_TRACE("--Reset command list.");
//		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));
//
//		INIT_TRACE("--Create ds resource barrier.");
//		_pCommandList->ResourceBarrier(
//			1,
//			&CD3DX12_RESOURCE_BARRIER::Transition(_pDepthStencil.Get(),
//				D3D12_RESOURCE_STATE_COMMON,
//				D3D12_RESOURCE_STATE_DEPTH_WRITE
//			)
//		);
//
//		INIT_TRACE("--Close command list.");
//		MCThrowIfFailed(_pCommandList->Close());
//
//		INIT_TRACE("--Execute.");
//		ID3D12CommandList* pCommandList = _pCommandList.Get();
//		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);
//
//		INIT_TRACE("--Flush command queue.");
//		FlushCommandQueue();
//
//		INIT_TRACE("End init finalize.");
//	}
//
//	void MCD3D::InitMatrices() {
//		INIT_TRACE("Init matrices.");
//
//		_worldMatrix      = MCMath::Identity4x4();
//		_viewMatrix       = MCMath::Identity4x4();
//		_projectionMatrix = MCMath::Identity4x4();
//
//		INIT_TRACE("End init matrices.");
//	}
//
//	/*
//		Examine the window width and height in _initialConfiguration. Throw an exception if the values do not
//		fall within an acceptable range.
//	*/
//	void MCD3D::EnsureValidWindowConfig(const RENDER_CONFIG& renderConfig) const {
//		if (renderConfig.DISPLAY_OUTPUT_WIDTH <= 0
//			|| renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_WIDTH) {
//			INIT_ERROR("Detected an invalid window width ({0:d}) in the config file.", renderConfig.DISPLAY_OUTPUT_WIDTH);
//			MCTHROW("Detected an invalid window width in the config file.");
//		}
//
//		if (renderConfig.DISPLAY_OUTPUT_HEIGHT <= 0
//			|| renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_HEIGHT) {
//			INIT_ERROR("Detected an invalid window height ({0:d}) in the config file.", renderConfig.DISPLAY_OUTPUT_HEIGHT);
//			MCTHROW("Detected an invalid window height in the config file.");
//		}
//	}
//
//#pragma endregion 
//
//#pragma region Test
//
//	void MCD3D::InitTest() {
//		INIT_TRACE("Begin test initialization.");
//
//		InitBoxGeometry();
//		/*InitBoxGeometryViews();*/
//		InitBoxRootSignature();
//		InitShaders();
//
//		INIT_TRACE("--Init input layout.");
//		_pElementLayoutDescriptions[0].SemanticName         = "POSITION";
//		_pElementLayoutDescriptions[0].SemanticIndex        = 0;
//		_pElementLayoutDescriptions[0].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
//		_pElementLayoutDescriptions[0].InputSlot            = 0;
//		_pElementLayoutDescriptions[0].AlignedByteOffset    = 0;
//		_pElementLayoutDescriptions[0].InputSlotClass       = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
//		_pElementLayoutDescriptions[0].InstanceDataStepRate = 0;
//		
//		_pElementLayoutDescriptions[1].SemanticName         = "COLOR";
//		_pElementLayoutDescriptions[1].SemanticIndex        = 0;
//		_pElementLayoutDescriptions[1].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
//		_pElementLayoutDescriptions[1].InputSlot            = 0;
//		_pElementLayoutDescriptions[1].AlignedByteOffset    = 12;
//		_pElementLayoutDescriptions[1].InputSlotClass       = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
//		_pElementLayoutDescriptions[1].InstanceDataStepRate = 0;
//
//		InitBoxPSO();
//
//		INIT_TRACE("End test initialization.");
//	}
//
//	/*
//		Initialize the cube geometry, create an upload buffer for the vertex and index buffers, copy
//		the vertexes and indexes into the buffer.
//	*/
//	void MCD3D::InitBoxGeometry() {
//		INIT_TRACE("Begin test geometry initialization.");
//
//		//MCVertex1Color pVerts[8]     = {};
//		//std::uint16_t  pIndicies[36] = {};
//
//		INIT_TRACE("--Generating cube geometry.");
//		//MCTest::GenerateTestCube(
//		//	0.0f, 0.0f, 0.0f,
//		//	1.0f, 1.0f, 1.0f,
//		//	pVerts,    sizeof(pVerts),
//		//	pIndicies, sizeof(pIndicies)
//		//);
//
//		std::vector<MCVertex1Color> verts;
//		std::vector<std::uint16_t>  indicies;
//
//		MCTest::GenerateTestSphere(
//			1.5f,
//			1.0f, 0.0f, 0.0f,
//			&verts,
//			&indicies,
//			4
//		);
//
//		MCVertex1Color cubeVerts[8];
//		uint16_t cubeIndicies[36];
//
//		MCTest::GenerateTestCube(0.0f, 0.0f, 0.0f, 1, 1, 1, cubeVerts, sizeof(cubeVerts), cubeIndicies, sizeof(cubeIndicies));
//
//		/////////////////////////////////////////////////////////////////////////
//		// COPY The vertexes into the _pBoxVerts upload buffer.
//
//		INIT_TRACE("--Reset command allocator.");
//		MCThrowIfFailed(_pCommandAllocator->Reset());
//
//		INIT_TRACE("--Reset command list.");
//		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));
//
//		INIT_TRACE("--Creating cube mesh resource.");
//
//		_pSphereMesh = std::make_unique<MCStaticMesh16<MCVertex1Color>>("Sphere");
//
//		_pCubeMesh = std::make_unique<MCStaticAllocatedMesh16<MCVertex1Color>>("Cube");
//
//		auto disposer = _pSphereMesh->Upload(
//			_pDevice,
//			_pCommandList.Get(),
//			&verts[0], (UINT)(sizeof(MCVertex1Color) * verts.size()),
//			&indicies[0], (std::uint16_t)(sizeof(uint16_t) * indicies.size())
//		);
//
//		/*_pCubeMesh->Upload(
//			_pDevice,
//			_pCommandList.Get(),
//			cubeVerts,
//			sizeof(cubeVerts),
//			cubeIndicies,
//			sizeof(cubeIndicies)
//		);*/
//
//		_pCubeMesh->Upload(
//			cubeVerts,
//			sizeof(cubeVerts),
//			cubeIndicies,
//			sizeof(cubeIndicies)
//		);
//
//		INIT_TRACE("--Close command list.");
//		MCThrowIfFailed(_pCommandList->Close());
//
//		INIT_TRACE("--Execute.");
//		ID3D12CommandList* pCommandList = _pCommandList.Get();
//		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);
//
//		/////////////////////////////////////////////////////////////////////////
//		// Copy the indexes into the _pBoxIndicies
//
//		INIT_TRACE("--Letting the GPU catch up to release the upload buffer.");
//		FlushCommandQueue();
//
//		INIT_TRACE("End box geometry initialization.");
//	}
//
//	void MCD3D::InitShaders() {
//		INIT_TRACE("Begin shader init.");
//
//		INIT_TRACE("--Load standard vertex shader.");
//		_hVertexShader = MCShaderManager::Instance()->Load("StandardVertex.cso");
//
//		INIT_TRACE("-- Load standard pixel shader.");
//		_hPixelShader = MCShaderManager::Instance()->Load("StandardPixel.cso");
//
//		INIT_TRACE("End shader init.");
//	}
//
//	void MCD3D::InitBoxRootSignature() {
//		INIT_TRACE("Begin box root signature initialize.");
//
//		CD3DX12_ROOT_PARAMETER pSlotRootParameter[1];
//
//		// Creating a single parameter that will point to the constant buffer. 
//		CD3DX12_DESCRIPTOR_RANGE constantBufferViewTable;
//		constantBufferViewTable.Init(
//			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
//			1, // number of DESCRIPTORS
//			0 // base shader register
//		);
//
//		pSlotRootParameter[0].InitAsDescriptorTable(
//			1, // number of descriptor ranges,
//			&constantBufferViewTable
//		);
//
//		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(
//			1, // number of root parameters,
//			pSlotRootParameter,
//			0, // number of static samplers
//			nullptr, // static sampler description
//			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
//		);
//
//		// Create a root signature with a single slot which pointer to a
//		// descriptor range consisting of a single constant buffer.
//		ComPtr<ID3DBlob> serializedRootSignature = nullptr;
//		ComPtr<ID3DBlob> errorBlob               = nullptr;
//		MCThrowIfFailed(D3D12SerializeRootSignature(
//			&rootSignatureDescription,
//			D3D_ROOT_SIGNATURE_VERSION_1,
//			&serializedRootSignature,
//			&errorBlob
//		));
//
//		MCThrowIfFailed(_pDevice->CreateRootSignature(
//			0,
//			serializedRootSignature->GetBufferPointer(),
//			serializedRootSignature->GetBufferSize(),
//			__uuidof(_pBoxRootSignature),
//			&_pBoxRootSignature
//		));
//
//		INIT_TRACE("End box root signature initialize.");
//	}
//
//	void MCD3D::InitBoxPSO() {
//		INIT_TRACE("Begin box pipeline state object init.");
//
//		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
//		ZeroMemory(&psoDesc, sizeof(psoDesc));
//
//		auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//		rasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
//		rasterizerState.CullMode = D3D12_CULL_MODE_BACK;
//		rasterizerState.AntialiasedLineEnable = false;
//
//		psoDesc.InputLayout = { _pElementLayoutDescriptions, _countof(_pElementLayoutDescriptions) };
//		psoDesc.pRootSignature = _pBoxRootSignature.Get();
//		psoDesc.VS = MCShaderManager::Instance()->GetByteCode(_hVertexShader);
//		psoDesc.PS = MCShaderManager::Instance()->GetByteCode(_hPixelShader);
//		psoDesc.RasterizerState = rasterizerState;
//		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//		psoDesc.SampleMask = UINT_MAX;
//		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//		psoDesc.NumRenderTargets = 1;
//		psoDesc.RTVFormats[0] = BACK_BUFFER_FORMAT;
//
//		// TODO: As of now we do not have support for multi-sampling
//		psoDesc.SampleDesc.Count = 1;
//		psoDesc.SampleDesc.Quality = 0;
//
//		psoDesc.DSVFormat = MC_DEPTH_STENCIL_FORMAT;
//
//		_pDevice->CreateGraphicsPipelineState(
//			&psoDesc,
//			__uuidof(_pBoxPSO),
//			&_pBoxPSO
//		);
//
//		_pBoxPSO->SetName(L"Box PSO");
//
//		INIT_TRACE("End box pipeline state object init.");
//	}
//
//#pragma endregion
//
//#pragma region QuickDraw
//
//	void MCD3D::TestUpdate(const MCFrame3D* pFrame) {
//
//		DirectX::XMVECTOR target = DirectX::XMVectorSet(
//										pFrame->Camera.LookAt.x,
//										pFrame->Camera.LookAt.y,
//										pFrame->Camera.LookAt.z,
//										0.0f
//								   );
//		DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(
//										pFrame->Camera.Position.x,
//										pFrame->Camera.Position.y, 
//										pFrame->Camera.Position.z,
//										1.0f
//								   );
//		DirectX::XMVECTOR up        = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(cameraPos, target, up);
//		DirectX::XMStoreFloat4x4(&_viewMatrix, view);
//
//		// TODO::
//		// Aspect ratio needs to be tracked
//		float aspectRatio = MCREGlobals::pMCDXGI->GetAspectRatio();
//		DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(0.25f*3.14159f, aspectRatio, 1.0f, 1000.0f);
//		DirectX::XMStoreFloat4x4(&_projectionMatrix, proj);
//
//		DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&_worldMatrix);
//
//		DirectX::XMMATRIX worldViewProj = world*view*proj;
//
//		ObjectConstants oc;
//		DirectX::XMStoreFloat4x4(&oc.WorldViewProj, XMMatrixTranspose(worldViewProj));
//		oc.time = pFrame->Time;
//
//		// TODO:
//		// Tho CopyData function does not appear to take into account the fact that oc is smaller
//		// than the internal elementsize.
//
//		_pObjectConstantBuffer->CopyData(0, oc);
//	}
//
//	void MCD3D::QuickDraw() {
//
//		if (MCREGlobals::pMCDXGI->IsResizeQueued())
//			Resize();
//
//		MCThrowIfFailed(_pCommandAllocator->Reset());
//
//		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), _pBoxPSO.Get()));
//
//		_pCommandList->RSSetViewports(1, &_viewPort);
//		_pCommandList->RSSetScissorRects(1, &_scissorRect);
//
//		UINT currentBackBufferIndex = MCREGlobals::pMCDXGI->GetCurrentBackBufferIndex();
//
//		_pCommandList->ResourceBarrier(
//			1,
//			&CD3DX12_RESOURCE_BARRIER::Transition(
//				_ppRenderTargets[currentBackBufferIndex].Get(),
//				D3D12_RESOURCE_STATE_PRESENT,
//				D3D12_RESOURCE_STATE_RENDER_TARGET
//			)
//		);
//		
//		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
//		rtvHeapHandle.Offset(currentBackBufferIndex, _RTVDescriptorSize);
//
//		FLOAT colorBlack[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
//
//		_pCommandList->ClearRenderTargetView(
//			rtvHeapHandle,
//			colorBlack,
//			0,
//			nullptr
//		);
//
//		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
//
//		_pCommandList->ClearDepthStencilView(
//			dsvHeapHandle,
//			D3D12_CLEAR_FLAG_DEPTH,
//			1.0F,
//			0,
//			0,
//			nullptr
//		);
//
//		_pCommandList->OMSetRenderTargets(
//			1,
//			&rtvHeapHandle,
//			true,
//			&dsvHeapHandle
//		);
//
//		ID3D12DescriptorHeap* descriptorHeaps[] = { _pCBVDescriptorHeap.Get() };
//		_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//
//		_pCommandList->SetGraphicsRootSignature(_pBoxRootSignature.Get());
//		_pCommandList->SetGraphicsRootDescriptorTable(0, _pCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
//
//		_pCommandList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		_pSphereMesh->SetIABuffers(_pCommandList.Get());
//		_pSphereMesh->DrawSubMesh("Sphere", _pCommandList.Get());
//
//		//_pCubeMesh->SetIABuffers(_pCommandList.Get());
//		//_pCubeMesh->DrawSubMesh("Cube", _pCommandList.Get());
//
//		_pCubeMesh->Draw(_pCommandList.Get());
//
//		_pCommandList->ResourceBarrier(
//			1,
//			&CD3DX12_RESOURCE_BARRIER::Transition(
//				_ppRenderTargets[currentBackBufferIndex].Get(),
//				D3D12_RESOURCE_STATE_RENDER_TARGET,
//				D3D12_RESOURCE_STATE_PRESENT
//			)
//		);
//
//		MCThrowIfFailed(_pCommandList->Close());
//
//		ID3D12CommandList* cmdsLists[] = { _pCommandList.Get() };
//		_pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
//
//		MCThrowIfFailed(MCREGlobals::pMCDXGI->Present());
//
//		FlushCommandQueue();
//
//		
//	}
//
//#pragma endregion
//
//#pragma region Render
//
//	void MCD3D::RenderFrame(const MCFrame3D* pFrame) {
//		TestUpdate(pFrame);
//		QuickDraw();
//	}
//
//#pragma endregion
//
//#pragma region fencing
//
//	/*
//		Get the current completed value of _pFence.
//	*/
//	unsigned __int64 MCD3D::GetCurrentFenceValue() const {
//		return _pFence->GetCompletedValue();
//	}
//
//	/*
//		Block the calling thread until _pFence reaches fenceValue.
//	*/
//	void MCD3D::WaitForFenceValue(unsigned __int64 fenceValue) const {
//		if (_pFence->GetCompletedValue() < fenceValue) {
//			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
//
//			MCThrowIfFailed(_pFence->SetEventOnCompletion(fenceValue, eventHandle));
//			
//			WaitForSingleObject(eventHandle, INFINITE);
//			CloseHandle(eventHandle);
//		}
//	}
//
//	/*
//		increment _currentFence, then use the new value to signal the command queue.
//	*/
//	unsigned __int64 MCD3D::GetNewFenceValue() {
//		unsigned __int64 nextFence = _currentFence.fetch_add(1);
//		MCThrowIfFailed(_pCommandQueue->Signal(_pFence.Get(), nextFence));
//		return nextFence;
//	}
//
//	/*
//		Create a new fence, then block until that fence completes.
//	*/
//	void MCD3D::FlushCommandQueue()
//	{
//		auto nextFenceValue = GetNewFenceValue();
//		WaitForFenceValue(nextFenceValue);
//	}
//
//#pragma endregion
//
//#pragma region Command list support
//
//	void MCD3D::ExecuteCommandList(ID3D12CommandList *pCommandList) const {
//		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);
//	}
//
//	void MCD3D::ExecuteCommandLists(int numCommandLists, ID3D12CommandList* const *pCommandLists) const {
//		_pCommandQueue->ExecuteCommandLists(numCommandLists, pCommandLists);
//	}
//
//#pragma endregion
//
//#pragma region Utilities	
//	
//
//	void MCD3D::Resize() {
//
//		/*
//			The general steps to resize the swap chain buffers are as follows:
//
//			1) Release resources attached to the swap chain
//				a) Flush the command queue.
//				b) Reset the command list.
//				c) Release the frame buffer RESOURCES
//				d) Release the depth stencil buffer RESOURCE
//
//			2) Resize the buffers.
//
//			3) Re-initialize resources.
//				a) re-initialize frame buffer resources.
//				b) re-initialize stencil buffer resource.
//
//		*/
//		
//
//		assert(_pDevice);
//		assert(MCREGlobals::pMCDXGI);
//		assert(_pCommandAllocator);
//
//		// Make sure the command queue is flushed.
//		FlushCommandQueue();
//
//		// Reset the command list.
//		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));
//
//		for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
//			_ppRenderTargets[i].Reset();
//
//		_pDepthStencil.Reset();
//
//		MCREGlobals::pMCDXGI->ForceResize();
//
//		InitRenderTargets();
//		InitRenderTargetViews();
//
//		InitDepthStencilBuffer();
//		InitDepthStencilBufferView();
//
//		_pCommandList->ResourceBarrier(
//			1,
//			&CD3DX12_RESOURCE_BARRIER::Transition(_pDepthStencil.Get(),
//				D3D12_RESOURCE_STATE_COMMON,
//				D3D12_RESOURCE_STATE_DEPTH_WRITE
//			)
//		);
//
//		MCThrowIfFailed(_pCommandList->Close());
//
//		ID3D12CommandList* pCommandList = _pCommandList.Get();
//		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);
//
//		FlushCommandQueue();
//
//		int newWidth, newHeight;
//		MCREGlobals::pMCDXGI->GetFrameBufferSize(&newWidth, &newHeight);
//
//		_viewPort = {};
//		_viewPort.TopLeftX = 0.0f;
//		_viewPort.TopLeftY = 0.0f;
//		_viewPort.Width  = static_cast<float>(newWidth);
//		_viewPort.Height = static_cast<float>(newHeight);
//		_viewPort.MinDepth = 0.0f;
//		_viewPort.MaxDepth = 1.0f;
//
//		_scissorRect.top    = 0;
//		_scissorRect.left   = 0;
//		_scissorRect.right  = newWidth;
//		_scissorRect.bottom = newHeight;
//	}
//
//#pragma endregion

#pragma endregion

#pragma region CtorDtor

MCD3D::MCD3D()  {
	MC_INFO("Begin render initialization.");

	MC_INFO("atomic fence is lock free: {0:d}", (int)_currentFence.is_lock_free());

	assert(MCREGlobals::pMCDXGI);


	EnsureValidWindowConfig(MCREGlobals::InitialConfiguration);

	// initialize the current fence value
	_currentFence = 0;

	Init3DDevice();
	InitFence();
	InitCommandQueue();
	InitCommandAllocator();
	InitCommandList();
	InitSwapChain();
	InitDescriptorHeaps();
	InitRenderTargets();
	InitRenderTargetViews();
	InitDepthStencilBuffers();
	InitDepthStencilBufferViews();
	InitMatrices();
	InitFinalize();

	MC_INFO("End render initialization.");
}

MCD3D::~MCD3D() {}

#pragma endregion 

#pragma region Initialization

void MCD3D::InitFence() {
	INIT_TRACE("Begin d3d12 fence init.");
	MCThrowIfFailed(_pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		__uuidof(_pFence),
		&_pFence
	));
	_pFence->SetName(L"MCD3D fence.");
	INIT_TRACE("End d3d12 fence init.");
}

void MCD3D::InitCommandQueue() {
	INIT_TRACE("Begin d3d12 command queue init.");

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	MCThrowIfFailed(_pDevice->CreateCommandQueue(
		&queueDesc,
		__uuidof(_pCommandQueue),
		&_pCommandQueue
	));

	_pCommandQueue->SetName(L"Command Queue");

	INIT_TRACE("End d3d12 command queue init.");
}

void MCD3D::InitCommandAllocator() {
	INIT_TRACE("Begin d3d12 command allocator queue init.");

	MCThrowIfFailed(_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(_pCommandAllocator),
		&_pCommandAllocator
	));

	_pCommandAllocator->SetName(L"Command Allocator");

	INIT_TRACE("End d3d12 command allocator queue inti.");
}

void MCD3D::InitCommandList() {
	INIT_TRACE("Begin d3d12 command list init.");

	MCThrowIfFailed(_pDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_pCommandAllocator.Get(),
		nullptr,
		__uuidof(_pCommandList),
		&_pCommandList
	));

	_pCommandList->SetName(L"Default command list");

	// The command queue should begin in a closed state because the render loop
	// will expect it to be in a closed state.
	_pCommandList->Close();

	INIT_TRACE("End d3d12 command list init.");
}

void MCD3D::InitSwapChain() {
	INIT_TRACE("Begin swapchain init.");

	MCREGlobals::pMCDXGI->CreateConfiguredOrDefaltSwapchain(_pCommandQueue.Get());

	INIT_TRACE("End swapchain init.");
}

void MCD3D::Init3DDevice() {
	INIT_TRACE("Begin 3d device init.");

	_pDevice = MCREGlobals::pMCDXGI->CreateConfiguredOrDefault3DDevice();

	INIT_TRACE("End 3d device init.");
}

void MCD3D::InitDescriptorHeaps() {
	INIT_TRACE("Begin descriptor heap init.");

	// Render Target Descriptor Heap ****************************

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

	rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
	rtvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(_pRTVDescriptorHeap), &_pRTVDescriptorHeap));
	
	// Depth Stencil Buffer Descriptor Heap *************************

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

	
	dsvHeapDesc.NumDescriptors = DEPTH_BUFFER_COUNT; 
	dsvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	MCThrowIfFailed(_pDevice->CreateDescriptorHeap(&dsvHeapDesc, __uuidof(_pDSVDescriptorHeap), &_pDSVDescriptorHeap));

	_RTVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_DSVDescriptorSize = _pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	INIT_TRACE("End descriptor heap init.");
}

void MCD3D::InitRenderTargets() {
	INIT_TRACE("Begin init render targets.");

	// Create a render target view for each swap chain buffer
	for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
		MCREGlobals::pMCDXGI->GetFrameBuffer(n, __uuidof(_ppRenderTargets[n]), &_ppRenderTargets[n]);
	}

	INIT_TRACE("End init render targets.");
}

void MCD3D::InitRenderTargetViews() {
	INIT_TRACE("Begin init RTV Heap.");

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
		_pDevice->CreateRenderTargetView(_ppRenderTargets[n].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, _RTVDescriptorSize);
	}

	INIT_TRACE("End init RTV Heap.");
}

void MCD3D::InitDepthStencilBuffers() {
	INIT_TRACE("Init depth stencil buffers.");

	int width, height;

	MCREGlobals::pMCDXGI->GetFrameBufferSize(&width, &height);

	D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilBufferDesc.Alignment = 0;
	depthStencilBufferDesc.Width     = width;
	depthStencilBufferDesc.Height    = height;
	depthStencilBufferDesc.DepthOrArraySize = 1;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.Format    = MC_DEPTH_STENCIL_FORMAT;
	depthStencilBufferDesc.SampleDesc.Count = 1;

	depthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilBufferDesc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;

	optClear.Format               = MC_DEPTH_STENCIL_FORMAT;
	optClear.DepthStencil.Depth   = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	for (unsigned int n = 0; n < DEPTH_BUFFER_COUNT; ++n) {
		INIT_TRACE("**Initializing depth stencil {0:d}.", n);
		MCThrowIfFailed(_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilBufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			__uuidof(_ppDepthStencils[n]),
			&_ppDepthStencils[n]
		));
		_ppDepthStencils[n]->SetName((std::wstring(L"Depth stencil") + std::to_wstring(n)).c_str());
		INIT_TRACE("**Initialized depth stencil {0:d}.", n);
	}

	INIT_TRACE("End init depth stencil buffer.");
}

void MCD3D::InitDepthStencilBufferViews() {
	INIT_TRACE("Begin init depth stencil buffer views.");

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (unsigned int n = 0; n < DEPTH_BUFFER_COUNT; ++n) {
		INIT_TRACE("**Initialize depth stencil view {0:d}.", n);
		_pDevice->CreateDepthStencilView(
			_ppDepthStencils[n].Get(),
			nullptr,
			dsvHeapHandle
		);
		dsvHeapHandle.Offset(1, _DSVDescriptorSize);
		INIT_TRACE("**Initialized depth stencil view{0:d}.", n);
	}

	INIT_TRACE("End init depth stencil buffer view.");
}

void MCD3D::InitFinalize() {
	INIT_TRACE("Begin init finalize.");

	INIT_TRACE("--Reset command allocator.");
	MCThrowIfFailed(_pCommandAllocator->Reset());

	INIT_TRACE("--Reset command list.");
	MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

	INIT_TRACE("--Create ds resource barriers.");

	for (unsigned int n; n < DEPTH_BUFFER_COUNT; ++n) {
		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(_ppDepthStencils[n].Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			)
		);
	}

	INIT_TRACE("--Close command list.");
	MCThrowIfFailed(_pCommandList->Close());

	INIT_TRACE("--Execute.");
	ID3D12CommandList* pCommandList = _pCommandList.Get();
	_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

	INIT_TRACE("--Flush command queue.");
	FlushCommandQueue();

	INIT_TRACE("End init finalize.");
}

#pragma endregion 

#pragma region RenderTarget / DepthStencil access

/* Access the specific render target associated with 'index' */
ID3D12Resource* MCD3D::GetRenderTarget(unsigned int index) {
	assert(index < FRAME_BUFFER_COUNT);
	return _ppRenderTargets[index].Get();
}

/* Access the CPU descriptor handle for the render target associated with 'index' */
D3D12_CPU_DESCRIPTOR_HANDLE MCD3D::GetRenderTargetCPUHandle(unsigned int index) {
	assert(index < FRAME_BUFFER_COUNT);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.Offset(index, _RTVDescriptorSize);
	return (D3D12_CPU_DESCRIPTOR_HANDLE)handle;
}

/* Access the GPU descriptor handle for the render target associated with 'index' */
D3D12_GPU_DESCRIPTOR_HANDLE MCD3D::GetRenderTargetGPUHandle(unsigned int index) {
	assert(index < FRAME_BUFFER_COUNT);
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle(_pRTVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	handle.Offset(index, _RTVDescriptorSize);
	return (D3D12_GPU_DESCRIPTOR_HANDLE)handle;
}

/* Access to the DepthStencil associated with 'index' */
ID3D12Resource* MCD3D::GetDepthStencil(unsigned int index) {
	assert(index < DEPTH_BUFFER_COUNT);
	return _ppDepthStencils[index].Get();
}

/* Access the CPU descriptor handle for the depth stencil associated with 'index' */
D3D12_CPU_DESCRIPTOR_HANDLE MCD3D::GetDepthStencilCPUHandle(unsigned int index) {
	assert(index < DEPTH_BUFFER_COUNT);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(index, _DSVDescriptorSize);
	return (D3D12_CPU_DESCRIPTOR_HANDLE)handle;
}

/* Access the CPU descriptor handle for the depth stencil associated with 'index' */
D3D12_GPU_DESCRIPTOR_HANDLE MCD3D::GetDepthStencilGPUHandle(unsigned int index) {
	assert(index < DEPTH_BUFFER_COUNT);
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle(_pDSVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	handle.Offset(index, _DSVDescriptorSize);
	return (D3D12_GPU_DESCRIPTOR_HANDLE)handle;
}

#pragma endregion


#pragma region fencing

/*
Get the current completed value of _pFence.
*/
unsigned __int64 MCD3D::GetCurrentFenceValue() const {
	return _pFence->GetCompletedValue();
}

/*
Block the calling thread until _pFence reaches fenceValue.
*/
void MCD3D::WaitForFenceValue(unsigned __int64 fenceValue) const {
	if (_pFence->GetCompletedValue() < fenceValue) {
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		MCThrowIfFailed(_pFence->SetEventOnCompletion(fenceValue, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

/*
increment _currentFence, then use the new value to signal the command queue.
*/
unsigned __int64 MCD3D::GetNewFenceValue() {
	unsigned __int64 nextFence = _currentFence.fetch_add(1);
	MCThrowIfFailed(_pCommandQueue->Signal(_pFence.Get(), nextFence));
	return nextFence;
}

/*
Create a new fence, then block until that fence completes.
*/
void MCD3D::FlushCommandQueue() {
	auto nextFenceValue = GetNewFenceValue();
	WaitForFenceValue(nextFenceValue);
}

#pragma endregion

#pragma region Command list support

void MCD3D::ExecuteCommandList(ID3D12CommandList *pCommandList) const {
	_pCommandQueue->ExecuteCommandLists(1, &pCommandList);
}

void MCD3D::ExecuteCommandLists(int numCommandLists, ID3D12CommandList* const *pCommandLists) const {
	_pCommandQueue->ExecuteCommandLists(numCommandLists, pCommandLists);
}

#pragma endregion

#pragma region Utilities	


void MCD3D::Resize() {

	/*
	The general steps to resize the swap chain buffers are as follows:

	1) Release resources attached to the swap chain
	a) Flush the command queue.
	b) Reset the command list.
	c) Release the frame buffer RESOURCES
	d) Release the depth stencil buffer RESOURCE

	2) Resize the buffers.

	3) Re-initialize resources.
	a) re-initialize frame buffer resources.
	b) re-initialize stencil buffer resource.

	*/


	assert(_pDevice);
	assert(MCREGlobals::pMCDXGI);
	assert(_pCommandAllocator);

	// Make sure the command queue is flushed.
	FlushCommandQueue();

	// Reset the command list.
	MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
		_ppRenderTargets[i].Reset();

	_pDepthStencil.Reset();

	MCREGlobals::pMCDXGI->ForceResize();

	InitRenderTargets();
	InitRenderTargetViews();

	InitDepthStencilBuffer();
	InitDepthStencilBufferView();

	_pCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_pDepthStencil.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE
		)
	);

	MCThrowIfFailed(_pCommandList->Close());

	ID3D12CommandList* pCommandList = _pCommandList.Get();
	_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

	FlushCommandQueue();

	int newWidth, newHeight;
	MCREGlobals::pMCDXGI->GetFrameBufferSize(&newWidth, &newHeight);

	_viewPort = {};
	_viewPort.TopLeftX = 0.0f;
	_viewPort.TopLeftY = 0.0f;
	_viewPort.Width = static_cast<float>(newWidth);
	_viewPort.Height = static_cast<float>(newHeight);
	_viewPort.MinDepth = 0.0f;
	_viewPort.MaxDepth = 1.0f;

	_scissorRect.top = 0;
	_scissorRect.left = 0;
	_scissorRect.right = newWidth;
	_scissorRect.bottom = newHeight;
}

#pragma endregion
}
