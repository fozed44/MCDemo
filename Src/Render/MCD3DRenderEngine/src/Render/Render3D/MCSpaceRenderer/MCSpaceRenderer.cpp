#pragma once

#include "MCSpaceRenderer.h"
#include "../../../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../Common/MCRenderItem.h"

namespace MC {

#pragma region Ctor

	MCSpaceRenderer::MCSpaceRenderer(const std::string& name, unsigned int frameIndex, MCRenderScene* pScene) 
		: MCFrameRenderer3D(name, frameIndex, pScene) {
		InitializeSpaceRenderer();
	}

	MCSpaceRenderer::~MCSpaceRenderer() {

	}

#pragma endregion

#pragma region Initialization

	void MCSpaceRenderer::InitializeSpaceRenderer() {
		INIT_TRACE("Begin renderer {0} initialization.", Name());
		InitializeDescriptorHeaps();
		INIT_TRACE("End renderer {0} initialization.", Name());
	}

	void MCSpaceRenderer::InitializeDescriptorHeaps() {
		INIT_TRACE("Initializing CB_SR_UA for space renderer {0}.", Name());

		// Constant Buffer Descriptor Heap ***************************

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};

		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
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

	void MCSpaceRenderer::OnResizing() {
		MCThrowIfFailed(_pCommandAllocator->Reset());
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));
		_pRenderTarget->Release();
		_pRenderTarget = nullptr;
		MCThrowIfFailed(_pCommandList->Close());
	}

#pragma endregion


#pragma region Draw

	void MCSpaceRenderer::PrepareCommandLists(MCFrame *pVframe) {
		// Render should only be called by a render executer, on an executer thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::RENDERER_EXECUTER);

		std::unique_ptr<MCSpaceFrame> pFrame(static_cast<MCSpaceFrame*>(pVframe));
		assert(pFrame->FrameType == MC_FRAME_TYPE_MCFRAME_SPACE);

		MCThrowIfFailed(_pCommandAllocator->Reset());
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(
											pFrame->Camera.LookAt.x,
											pFrame->Camera.LookAt.y,
											pFrame->Camera.LookAt.z,
											0.0f
										 );

		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(
											pFrame->Camera.Position.x,
											pFrame->Camera.Position.y,
											pFrame->Camera.Position.z,
											1.0f
		                                   );

		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPosition, cameraTarget, up);

		DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(_fov, MCREGlobals::pMCDXGI->GetAspectRatio(), 1.0f, _farPlane);
		

		_pCommandList->RSSetViewports(1, &_viewPort);
		_pCommandList->RSSetScissorRects(1, &_scissorRect);

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				_pRenderTarget,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			)
		);

		FLOAT colorBlack[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		_pCommandList->ClearRenderTargetView(
			_hCPURenderTarget,
			colorBlack,
			0,
			nullptr
		);		

		_pCommandList->ClearDepthStencilView(
			_hCPUDepthStencil,
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0,
			0,
			nullptr
		);

		_pCommandList->OMSetRenderTargets(
			1,
			&_hCPURenderTarget,
			true,
			&_hCPUDepthStencil
		);

		ID3D12DescriptorHeap* descriptorHeaps[] = { _pCBVDescriptorHeap.Get() };
		//_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectX::XMMATRIX objectMatrix = DirectX::XMMatrixIdentity();

		auto pRenderItems = _pScene->GetRenderItems();

		for (int x = 0; x < pRenderItems->size(); ++x) {
			(*pRenderItems)[x]->Render(viewMatrix, projMatrix, objectMatrix, _pCommandList.Get());
		}

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				_pRenderTarget,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT
			)
		);

		_pCommandList->Close();
	}

	unsigned __int64 MCSpaceRenderer::ExecuteCommandLists() {
		MCREGlobals::pMCD3D->ExecuteCommandList(_pCommandList.Get());

		return MCREGlobals::pMCD3D->GetNewFenceValue();
	}

#pragma endregion

#pragma region Render options

	void MCSpaceRenderer::SetRenderOptions(const MCFrameRenderOptions& renderOptions) {
		// SetRenderOptions is not thread safe. It must be synchronized by some
		// outside force.
		_renderOptions = renderOptions;
	}

	MCFrameRenderOptions MCSpaceRenderer::GetRenderOptions() const {
		return _renderOptions;
	}

#pragma endregion

}
