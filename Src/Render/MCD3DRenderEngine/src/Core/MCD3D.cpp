#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "d3dx12.h"

#include "MCD3D.h"
#include "RenderConfig.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"

#include "TestFunctions.h"
#include <fstream>


#include "MCD3D12RenderUtilities.h"

#define MC_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace MC {

#pragma region CtorDtor

	MCD3D::MCD3D() 
		: _initialConfiguration{},
		  _pObjectConstantBuffer(nullptr),
		  _pElementLayoutDescriptions{},
		  _initialized{ false } {}

	MCD3D::~MCD3D() {}

#pragma endregion 

#pragma region Initialization

	void MCD3D::Initialize(const RENDER_CONFIG& renderConfig) {
		MC_INFO("Begin render initialization.");

		assert(MCDXGI::Instance()->Initialized());

		*const_cast<RENDER_CONFIG*>(&_initialConfiguration) = renderConfig;

		EnsureValidWindowConfig();

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
		InitDepthStencilBuffer();
		InitDepthStencilBufferView();
		InitConstantBuffer();
		InitConstantBufferView();
		InitViewPort();
		InitMatrices();
		InitFinalize();

		InitTest();

		/*
			Initialized() should now return true.
		*/
		_initialized = true;

		MC_INFO("End render initialization.");
	}

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
		queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
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

		MCDXGI::Instance()->CreateConfiguredOrDefaltSwapchain(_pCommandQueue.Get());

		INIT_TRACE("End swapchain init.");
	}

	void MCD3D::Init3DDevice() {
		INIT_TRACE("Begin 3d device init.");

		_pDevice = MCDXGI::Instance()->CreateConfiguredOrDefault3DDevice();

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

		// Constant Buffer Descriptor Heap ****************************

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

		cbvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT; // Number of back buffers.
		cbvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
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

	void MCD3D::InitRenderTargets() {
		INIT_TRACE("Begin init render targets.");

		// Create a render target view for each swap chain buffer
		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
			MCDXGI::Instance()->GetFrameBuffer(n, __uuidof(_ppRenderTargets[n]), &_ppRenderTargets[n]);
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

	void MCD3D::InitDepthStencilBuffer() {
		INIT_TRACE("Init depth stencil buffer.");

		int width, height;

		MCDXGI::Instance()->GetFrameBufferSize(&width, &height);

		D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
		depthStencilBufferDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilBufferDesc.Alignment        = 0;
		depthStencilBufferDesc.Width            = width;
		depthStencilBufferDesc.Height           = height;
		depthStencilBufferDesc.DepthOrArraySize = 1;
		depthStencilBufferDesc.MipLevels        = 1;
		depthStencilBufferDesc.Format           = MC_DEPTH_STENCIL_FORMAT;
		depthStencilBufferDesc.SampleDesc.Count = 1;

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

		_pDepthStencil->SetName(L"Depth stencil");

		INIT_TRACE("End init depth stencil buffer.");
	}

	void MCD3D::InitDepthStencilBufferView() {
		INIT_TRACE("Begin init depth stencil buffer view.");

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// TODO:
		//	Unsure about dsvHeapHandle
		_pDevice->CreateDepthStencilView(
			_pDepthStencil.Get(),
			nullptr,
			dsvHeapHandle
		);		

		INIT_TRACE("End init depth stencil buffer view.");
	}

	void MCD3D::InitConstantBuffer() {
		INIT_TRACE("Init constant buffer.");

		_pObjectConstantBuffer = std::make_unique<MCUploadBuffer<ObjectConstants>>(_pDevice, 1, true);

		INIT_TRACE("End init constant buffer.");
	}

	void MCD3D::InitConstantBufferView() {
		INIT_TRACE("Init constant buffer view");

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = _pObjectConstantBuffer->Resource()->GetGPUVirtualAddress();

		// For the moment we are just getting a view for the box.
		unsigned int boxConstantBufferCalculatedSize = _pObjectConstantBuffer->CalculatedSize();
		unsigned int boxConstantBufferIndex = 0;
		cbAddress += boxConstantBufferIndex * boxConstantBufferCalculatedSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC boxConstantBufferViewDesc = {};
		boxConstantBufferViewDesc.BufferLocation = cbAddress;
		boxConstantBufferViewDesc.SizeInBytes = boxConstantBufferCalculatedSize;

		_pDevice->CreateConstantBufferView(
			&boxConstantBufferViewDesc,
			_pCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);

		INIT_TRACE("End init constant buffer view.");
	}

	void MCD3D::InitViewPort() {
		INIT_TRACE("Begin init view port.");

		_viewPort = {};
		_viewPort.TopLeftX = 0.0f;
		_viewPort.TopLeftY = 0.0f;
		_viewPort.Width    = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_WIDTH);
		_viewPort.Height   = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_HEIGHT);
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		_scissorRect.top    = 0;
		_scissorRect.left   = 0;
		_scissorRect.right  = _initialConfiguration.DISPLAY_OUTPUT_WIDTH;
		_scissorRect.bottom = _initialConfiguration.DISPLAY_OUTPUT_HEIGHT;
		/*_pCommandList->RSSetViewports(1, &_viewPort);*/

		INIT_TRACE("End view port.");
	}

	void MCD3D::InitFinalize() {
		INIT_TRACE("Begin init finalize.");

		INIT_TRACE("--Reset command allocator.");
		MCThrowIfFailed(_pCommandAllocator->Reset());

		INIT_TRACE("--Reset command list.");
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		INIT_TRACE("--Create ds resource barrier.");
		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(_pDepthStencil.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_DEPTH_WRITE
			)
		);

		INIT_TRACE("--Close command list.");
		MCThrowIfFailed(_pCommandList->Close());

		INIT_TRACE("--Execute.");
		ID3D12CommandList* pCommandList = _pCommandList.Get();
		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

		INIT_TRACE("--Flush command queue.");
		FlushCommandQueue();

		INIT_TRACE("End init finalize.");
	}

	void MCD3D::InitMatrices() {
		INIT_TRACE("Init matrices.");

		_worldMatrix      = MCMath::Identity4x4();
		_viewMatrix       = MCMath::Identity4x4();
		_projectionMatrix = MCMath::Identity4x4();

		INIT_TRACE("End init matrices.");
	}

	/*
		Examine the window width and height in _initialConfiguration. Throw an exception if the values do not
		fall within an acceptable range.
	*/
	void MCD3D::EnsureValidWindowConfig() {
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

#pragma region Test

	void MCD3D::InitTest() {
		INIT_TRACE("Begin test initialization.");

		InitBoxGeometry();
		/*InitBoxGeometryViews();*/
		InitBoxRootSignature();
		InitShaders();

		INIT_TRACE("--Init input layout.");
		_pElementLayoutDescriptions[0].SemanticName         = "POSITION";
		_pElementLayoutDescriptions[0].SemanticIndex        = 0;
		_pElementLayoutDescriptions[0].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		_pElementLayoutDescriptions[0].InputSlot            = 0;
		_pElementLayoutDescriptions[0].AlignedByteOffset    = 0;
		_pElementLayoutDescriptions[0].InputSlotClass       = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		_pElementLayoutDescriptions[0].InstanceDataStepRate = 0;
		
		_pElementLayoutDescriptions[1].SemanticName         = "COLOR";
		_pElementLayoutDescriptions[1].SemanticIndex        = 0;
		_pElementLayoutDescriptions[1].Format               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		_pElementLayoutDescriptions[1].InputSlot            = 0;
		_pElementLayoutDescriptions[1].AlignedByteOffset    = 12;
		_pElementLayoutDescriptions[1].InputSlotClass       = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		_pElementLayoutDescriptions[1].InstanceDataStepRate = 0;

		InitBoxPSO();

		INIT_TRACE("End test initialization.");
	}

	/*
		Initialize the cube geometry, create an upload buffer for the vertex and index buffers, copy
		the vertexes and indexes into the buffer.
	*/
	void MCD3D::InitBoxGeometry() {
		INIT_TRACE("Begin test geometry initialization.");

		//MCVertex1Color pVerts[8]     = {};
		//std::uint16_t  pIndicies[36] = {};

		INIT_TRACE("--Generating cube geometry.");
		//MCTest::GenerateTestCube(
		//	0.0f, 0.0f, 0.0f,
		//	1.0f, 1.0f, 1.0f,
		//	pVerts,    sizeof(pVerts),
		//	pIndicies, sizeof(pIndicies)
		//);

		std::vector<MCVertex1Color> verts;
		std::vector<std::uint16_t>  indicies;

		MCTest::GenerateTestSphere(
			1.5f,
			1.0f, 0.0f, 0.0f,
			&verts,
			&indicies,
			4
		);

		MCVertex1Color cubeVerts[8];
		uint16_t cubeIndicies[36];

		MCTest::GenerateTestCube(0.0f, 0.0f, 0.0f, 1, 1, 1, cubeVerts, sizeof(cubeVerts), cubeIndicies, sizeof(cubeIndicies));

		/////////////////////////////////////////////////////////////////////////
		// COPY The vertexes into the _pBoxVerts upload buffer.

		INIT_TRACE("--Reset command allocator.");
		MCThrowIfFailed(_pCommandAllocator->Reset());

		INIT_TRACE("--Reset command list.");
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		INIT_TRACE("--Creating cube mesh resource.");

		_pSphereMesh = std::make_unique<MCStaticMesh16<MCVertex1Color>>("Sphere");

		_pCubeMesh = std::make_unique<MCDynamicMesh16<MCVertex1Color>>("Cube");

		auto disposer = _pSphereMesh->Upload(
			_pDevice,
			_pCommandList.Get(),
			&verts[0], sizeof(MCVertex1Color) * verts.size(),
			&indicies[0], (std::uint16_t)(sizeof(uint16_t) * indicies.size())
		);

		_pCubeMesh->Upload(
			_pDevice,
			_pCommandList.Get(),
			cubeVerts,
			sizeof(cubeVerts),
			cubeIndicies,
			sizeof(cubeIndicies)
		);

		INIT_TRACE("--Close command list.");
		MCThrowIfFailed(_pCommandList->Close());

		INIT_TRACE("--Execute.");
		ID3D12CommandList* pCommandList = _pCommandList.Get();
		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

		/////////////////////////////////////////////////////////////////////////
		// Copy the indexes into the _pBoxIndicies

		INIT_TRACE("--Letting the GPU catch up to release the upload buffer.");
		FlushCommandQueue();

		INIT_TRACE("End box geometry initialization.");
	}

	void MCD3D::InitShaders() {
		INIT_TRACE("Begin shader init.");

		INIT_TRACE("--Load standard vertex shader.");
		_hVertexShader = MCShaderManager::Instance()->Load("StandardVertex.cso");

		INIT_TRACE("-- Load standard pixel shader.");
		_hPixelShader = MCShaderManager::Instance()->Load("StandardPixel.cso");

		INIT_TRACE("End shader init.");
	}

	void MCD3D::InitBoxRootSignature() {
		INIT_TRACE("Begin box root signature initialize.");

		CD3DX12_ROOT_PARAMETER pSlotRootParameter[1];

		// Creating a single parameter that will point to the constant buffer. 
		CD3DX12_DESCRIPTOR_RANGE constantBufferViewTable;
		constantBufferViewTable.Init(
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			1, // number of DESCRIPTORS
			0 // base shader register
		);

		pSlotRootParameter[0].InitAsDescriptorTable(
			1, // number of descriptor ranges,
			&constantBufferViewTable
		);

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(
			1, // number of root parameters,
			pSlotRootParameter,
			0, // number of static samplers
			nullptr, // static sampler description
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		// Create a root signature with a single slot which pointer to a
		// descriptor range consisting of a single constant buffer.
		ComPtr<ID3DBlob> serializedRootSignature = nullptr;
		ComPtr<ID3DBlob> errorBlob               = nullptr;
		MCThrowIfFailed(D3D12SerializeRootSignature(
			&rootSignatureDescription,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSignature,
			&errorBlob
		));

		MCThrowIfFailed(_pDevice->CreateRootSignature(
			0,
			serializedRootSignature->GetBufferPointer(),
			serializedRootSignature->GetBufferSize(),
			__uuidof(_pBoxRootSignature),
			&_pBoxRootSignature
		));

		INIT_TRACE("End box root signature initialize.");
	}

	void MCD3D::InitBoxPSO() {
		INIT_TRACE("Begin box pipeline state object init.");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(psoDesc));

		auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerState.AntialiasedLineEnable = false;

		psoDesc.InputLayout = { _pElementLayoutDescriptions, _countof(_pElementLayoutDescriptions) };
		psoDesc.pRootSignature = _pBoxRootSignature.Get();
		psoDesc.VS = MCShaderManager::Instance()->GetByteCode(_hVertexShader);
		psoDesc.PS = MCShaderManager::Instance()->GetByteCode(_hPixelShader);
		psoDesc.RasterizerState = rasterizerState;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = BACK_BUFFER_FORMAT;

		// TODO: As of now we do not have support for multi-sampling
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		psoDesc.DSVFormat = MC_DEPTH_STENCIL_FORMAT;

		_pDevice->CreateGraphicsPipelineState(
			&psoDesc,
			__uuidof(_pBoxPSO),
			&_pBoxPSO
		);

		_pBoxPSO->SetName(L"Box PSO");

		INIT_TRACE("End box pipeline state object init.");
	}

#pragma endregion

#pragma region QuickDraw

	void MCD3D::TestUpdate(const MCFrame* pFrame) {

		DirectX::XMVECTOR target = DirectX::XMVectorSet(
										pFrame->LookAt.x,
										pFrame->LookAt.y,
										pFrame->LookAt.z,
										pFrame->LookAt.w
								   );
		DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(
										pFrame->CameraPosition.x,
										pFrame->CameraPosition.y, 
										pFrame->CameraPosition.z,
										pFrame->CameraPosition.w
								   );
		DirectX::XMVECTOR up        = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(cameraPos, target, up);
		DirectX::XMStoreFloat4x4(&_viewMatrix, view);

		// TODO::
		// Aspect ratio needs to be tracked
		float aspectRatio = MCDXGI::Instance()->GetAspectRatio();
		DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(0.25f*3.14159f, aspectRatio, 1.0f, 1000.0f);
		DirectX::XMStoreFloat4x4(&_projectionMatrix, proj);

		DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&_worldMatrix);

		DirectX::XMMATRIX worldViewProj = world*view*proj;

		ObjectConstants oc;
		DirectX::XMStoreFloat4x4(&oc.WorldViewProj, XMMatrixTranspose(worldViewProj));
		oc.time = pFrame->Time;

		// TODO:
		// Tho CopyData function does not appear to take into account the fact that oc is smaller
		// than the internal elementsize.

		_pObjectConstantBuffer->CopyData(0, oc);
	}

	void MCD3D::QuickDraw() {

		if (MCDXGI::Instance()->IsResizeQueued())
			Resize();

		MCThrowIfFailed(_pCommandAllocator->Reset());

		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), _pBoxPSO.Get()));

		_pCommandList->RSSetViewports(1, &_viewPort);
		_pCommandList->RSSetScissorRects(1, &_scissorRect);

		UINT currentBackBufferIndex = MCDXGI::Instance()->GetCurrentBackBufferIndex();

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				_ppRenderTargets[currentBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			)
		);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		rtvHeapHandle.Offset(currentBackBufferIndex, _RTVDescriptorSize);

		FLOAT colorBlack[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

		_pCommandList->ClearRenderTargetView(
			rtvHeapHandle,
			colorBlack,
			0,
			nullptr
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		_pCommandList->ClearDepthStencilView(
			dsvHeapHandle,
			D3D12_CLEAR_FLAG_DEPTH,
			1.0F,
			0,
			0,
			nullptr
		);

		_pCommandList->OMSetRenderTargets(
			1,
			&rtvHeapHandle,
			true,
			&dsvHeapHandle
		);

		ID3D12DescriptorHeap* descriptorHeaps[] = { _pCBVDescriptorHeap.Get() };
		_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		_pCommandList->SetGraphicsRootSignature(_pBoxRootSignature.Get());
		_pCommandList->SetGraphicsRootDescriptorTable(0, _pCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		_pCommandList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		_pSphereMesh->SetIABuffers(_pCommandList.Get());
		_pSphereMesh->DrawSubMesh("Sphere", _pCommandList.Get());

		_pCubeMesh->SetIABuffers(_pCommandList.Get());
		_pCubeMesh->DrawSubMesh("Cube", _pCommandList.Get());

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				_ppRenderTargets[currentBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			)
		);

		MCThrowIfFailed(_pCommandList->Close());

		ID3D12CommandList* cmdsLists[] = { _pCommandList.Get() };
		_pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		MCThrowIfFailed(MCDXGI::Instance()->Present());

		FlushCommandQueue();

		
	}

#pragma endregion

#pragma region Render

	void MCD3D::RenderFrame(const MCFrame* pFrame) {
		TestUpdate(pFrame);
		QuickDraw();
	}

#pragma endregion

#pragma region Utilities	

	void MCD3D::FlushCommandQueue()
	{
		// Advance the fence value to mark commands up to this fence point.
		_currentFence++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		MCThrowIfFailed(_pCommandQueue->Signal(_pFence.Get(), _currentFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (_pFence->GetCompletedValue() < _currentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			MCThrowIfFailed(_pFence->SetEventOnCompletion(_currentFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	

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
		assert(MCDXGI::Instance()->Initialized());
		assert(_pCommandAllocator);

		// Make sure the command queue is flushed.
		FlushCommandQueue();

		// Reset the command list.
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
			_ppRenderTargets[i].Reset();

		_pDepthStencil.Reset();

		MCDXGI::Instance()->ForceResize();

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
		MCDXGI::Instance()->GetFrameBufferSize(&newWidth, &newHeight);

		_viewPort = {};
		_viewPort.TopLeftX = 0.0f;
		_viewPort.TopLeftY = 0.0f;
		_viewPort.Width  = static_cast<float>(newWidth);
		_viewPort.Height = static_cast<float>(newHeight);
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		_scissorRect.top    = 0;
		_scissorRect.left   = 0;
		_scissorRect.right  = newWidth;
		_scissorRect.bottom = newHeight;
	}

#pragma endregion

	void MCD3D::OrderedStaticDestroy() {
		_hVertexShader.Destroy();
		_hPixelShader.Destroy();
	}
}
