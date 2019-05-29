#include "MCRenderItem.h"
#include "MCFrameRenderer.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

#pragma region ctor

	MCRenderItem::MCRenderItem(
		std::unique_ptr<MCIMesh> pMeshes,
		HShader                  hShader, 
		HRootSignature           hRootSignature,
		HPipelineState           hPipelineState
	)
		: _hShader{std::move(hShader)},
		  _hRootSignature{std::move(hRootSignature)},
		  _hPipelineState{std::move(hPipelineState)}
	{
		// Render items can only be created on the main thread
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		_meshes.push_back(std::move(pMeshes));

		/*_hObjectConstantBuffer = std::make_unique<MCUploadBuffer<ObjectConstants>>(
			MCREGlobals::pMCDXGI->Get3DDevice(),
			1,
			true
		);*/
		_hObjectConstantBuffer = MCREGlobals::pResourceManager->CreateDynamicConstantBuffer(nullptr, sizeof(ObjectConstants));
	}

	MCRenderItem::~MCRenderItem() { }

#pragma endregion

#pragma region Render

	MC_RESULT XM_CALLCONV MCRenderItem::Render(
		DirectX::FXMMATRIX         viewMatrix,
		DirectX::CXMMATRIX         projMatrix,
		DirectX::CXMMATRIX         worldMatrix,
		ID3D12GraphicsCommandList* pCommandList
	) {
		DirectX::XMMATRIX worldViewProj = worldMatrix * viewMatrix*projMatrix;

		ObjectConstants oc;
		DirectX::XMStoreFloat4x4(&oc.WorldViewProj, XMMatrixTranspose(worldViewProj));
		MCREGlobals::pResourceManager->MapDynamicConstantData(_hObjectConstantBuffer, &oc, sizeof(ObjectConstants));


		pCommandList->SetGraphicsRootSignature(MCREGlobals::pRSManager->GetRootSignature(_hRootSignature));
		pCommandList->SetPipelineState(MCREGlobals::pPSOManager->GetPipelineState(_hPipelineState));

		pCommandList->SetGraphicsRootConstantBufferView(0, MCREGlobals::pResourceManager->GetDynamicResource(_hObjectConstantBuffer)->GetGPUVirtualAddress());

		for (auto& mesh : _meshes) {
			mesh->Draw(pCommandList);
		}
		return MC_RESULT::OK;
	}

#pragma endregion
}
