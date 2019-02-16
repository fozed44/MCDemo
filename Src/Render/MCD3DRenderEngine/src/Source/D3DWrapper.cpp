#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../Headers/d3dx12.h"

#include "../Headers/D3DWrapper.h"
#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

#include "../Headers/TestFunctions.h"

#define MC_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT

namespace MC {

#pragma region CtorDtor

	D3DWrapper::D3DWrapper(const RENDER_CONFIG& renderConfig) 
		: _initialConfiguration(renderConfig), _pObjectConstantBuffer(nullptr){}

	D3DWrapper::~D3DWrapper() {}

#pragma endregion 

#pragma region Initialization

	void D3DWrapper::Init(std::shared_ptr<DXGIWrapper>& pDXGIWrapper) {
		MC_INFO("Begin render initialization.");

		assert(pDXGIWrapper->Initialized());

		EnsureValidWindowConfig();

		// Store a pointer to the dxgi wrapper.
		_pDXGIWrapper = pDXGIWrapper;

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

		_pCommandList->SetName(L"Default command list");

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

	void D3DWrapper::InitRenderTargets() {
		INIT_TRACE("Begin init render targets.");

		// Create a render target view for each swap chain buffer
		for (int n = 0; n < FRAME_BUFFER_COUNT; n++) {
			_pDXGIWrapper->GetFrameBuffer(n, __uuidof(_ppRenderTargets[n]), &_ppRenderTargets[n]);
		}

		INIT_TRACE("End init render targets.");
	}

	void D3DWrapper::InitRenderTargetViews() {
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

		_pDepthStencil->SetName(L"Depth stencil");

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

		INIT_TRACE("End init depth stencil buffer view.");
	}

	void D3DWrapper::InitConstantBuffer() {
		INIT_TRACE("Init constant buffer.");

		_pObjectConstantBuffer = std::make_unique<MCUploadBuffer<ObjectConstants>>(_pDevice, 1, true);

		INIT_TRACE("End init constant buffer.");
	}

	void D3DWrapper::InitConstantBufferView() {
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

	void D3DWrapper::InitViewPort() {
		INIT_TRACE("Begin init view port.");

		_viewPort = {};
		_viewPort.TopLeftX = 0.0f;
		_viewPort.TopLeftY = 0.0f;
		_viewPort.Width    = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_WIDTH);
		_viewPort.Height   = static_cast<float>(_initialConfiguration.DISPLAY_OUTPUT_HEIGHT);
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		/*_pCommandList->RSSetViewports(1, &_viewPort);*/

		INIT_TRACE("End view port.");
	}

	void D3DWrapper::InitFinalize() {
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

	void D3DWrapper::InitMatrices() {
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

#pragma region Test

	void D3DWrapper::InitTest() {
		INIT_TRACE("Begin test initialization.");

		InitBoxGeometry();
		InitBoxRootSignature();

		INIT_TRACE("End test initialization.");
	}

	void D3DWrapper::InitBoxGeometry() {
		INIT_TRACE("Begin box geometry initialization.");

		MCVertex1Color pVerts[8]     = {};
		std::uint16_t  pIndicies[36] = {};

		INIT_TRACE("--Generating cube geometry.");
		MCTest::GenerateTestCube(
			0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f,
			pVerts,    sizeof(pVerts),
			pIndicies, sizeof(pIndicies)
		);

		/*INIT_TRACE("--Reset command allocator.");
		MCThrowIfFailed(_pCommandAllocator->Reset());

		INIT_TRACE("--Reset command list.");
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));*/

		ComPtr<ID3D12Resource> uploadBuffer;

		INIT_TRACE("--Creating cube vertex resource.");

		ExecSync([_pBoxVerts, pVerts, uploadBuffer]() { _pBoxVerts = CreateDefaultBuffer(pVerts, sizeof(pVerts), uploadBuffer); })
		_pBoxVerts = CreateDefaultBuffer(pVerts, sizeof(pVerts), uploadBuffer);

		/*INIT_TRACE("--Close command list.");
		MCThrowIfFailed(_pCommandList->Close());

		INIT_TRACE("--Execute.");
		ID3D12CommandList* pCommandList = _pCommandList.Get();
		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

		INIT_TRACE("--Letting the GPU catch up to release the upload buffer.");
		FlushCommandQueue();*/

		INIT_TRACE("--Reset command allocator.");
		MCThrowIfFailed(_pCommandAllocator->Reset());

		INIT_TRACE("--Reset command list.");
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		INIT_TRACE("--Creating cube indexes.");
		_pBoxIndicies = CreateDefaultBuffer(pIndicies, sizeof(pIndicies), uploadBuffer);

		INIT_TRACE("--Close command list.");
		MCThrowIfFailed(_pCommandList->Close());

		INIT_TRACE("--Execute.");
		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

		INIT_TRACE("--Letting the GPU catch up to release the upload buffer.");
		FlushCommandQueue();


		INIT_TRACE("End box geometry initialization.");
	}

	void D3DWrapper::InitBoxRootSignature() {
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

#pragma endregion

#pragma region QuickDraw

	void D3DWrapper::QuickDraw() {

		MCThrowIfFailed(_pCommandAllocator->Reset());

		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		UINT currentBackBufferIndex = _pDXGIWrapper->GetCurrentBackBufferIndex();

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

		MCThrowIfFailed(_pDXGIWrapper->Swap());

		FlushCommandQueue();
	}

#pragma endregion

#pragma region Utilities

	/*
		Note:
			'uploadBuffer' has to be kept alive until AFTER the command list is executed.
	*/
	ComPtr<ID3D12Resource> D3DWrapper::CreateDefaultBuffer(void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer) {

		// the result.
		ComPtr<ID3D12Resource> defaultBuffer;

		MCThrowIfFailed(_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			__uuidof(defaultBuffer),
			&defaultBuffer
		));

		// In order to copy CPU memory data (initData) into a GPU default buffer (defaultBuffer) an intermediate upload
		// buffer must be created.
		MCThrowIfFailed(_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(uploadBuffer),
			&uploadBuffer
		));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
		);
		UpdateSubresources<1>(
			_pCommandList.Get(),
			defaultBuffer.Get(),
			uploadBuffer.Get(),
			0,
			0,
			1,
			&subResourceData
		);
		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_GENERIC_READ
			)
		);

		return defaultBuffer;
	}

	void D3DWrapper::FlushCommandQueue()
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

	void D3DWrapper::ExecSync(void (*func)()) {

		MCThrowIfFailed(_pCommandAllocator->Reset());

		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		func();

		MCThrowIfFailed(_pCommandList->Close());

		ID3D12CommandList* pCommandList = _pCommandList.Get();
		_pCommandQueue->ExecuteCommandLists(1, &pCommandList);

		INIT_TRACE("--Letting the GPU catch up to release the upload buffer.");
		FlushCommandQueue();
	}

#pragma endregion
}
