#include "MCFrameRenderer3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../Core/MCREGlobals.h";

namespace MC {

#pragma region ctor

	MCFrameRenderer3D::MCFrameRenderer3D(const std::string& name) 
		: MCFrameRendererBase(name) {
		// A frame renderer should only be CREATED on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);

		InitializeRenderer3D();
	}


	MCFrameRenderer3D::~MCFrameRenderer3D() {
		// A frame renderer should only be DESTROYED on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);
	}	

#pragma endregion

#pragma region Initialization

	void MCFrameRenderer3D::InitializeRenderer3D() {
		INIT_TRACE("Begin initialize renderer3d {0}.", Name());

		InitializeDescriptorHeaps();

		INIT_TRACE("End initialize renderer3D {0}.", Name());
	}

	void MCFrameRenderer3D::InitializeDescriptorHeaps() {
		INIT_TRACE("Initializing CB_SR_UA for renderer {0}.", Name());

		// Constant Buffer Descriptor Heap ***************************

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		MCThrowIfFailed( 
			MCREGlobals::pMCDXGI->Get3DDevice()->
			CreateDescriptorHeap(
				&cbvHeapDesc,
				__uuidof(_CBVDescriptorHeap),
				&_CBVDescriptorHeap
			)
		);

		_CBVDescriptorIncrementSize
			= MCREGlobals::pMCDXGI->Get3DDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		INIT_TRACE("Initialization complete for renderer {0}.", Name());
	}

#pragma endregion

#pragma region Draw

	void MCFrameRenderer3D::Render(const MCFrame3D& frame) {
		// Render should only be called by a render executer, on an executer thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER);

		MCThrowIfFailed(_pCommandAllocator->Reset());
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		_pCommandList->RSSetViewports   (1, &_viewPort);
		_pCommandList->RSSetScissorRects(1, &_scissorRect);

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				_pRenderTargetAlias,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			)
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(p)

	}

#pragma endregion
}