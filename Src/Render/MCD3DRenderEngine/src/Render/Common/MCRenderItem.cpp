#include "MCRenderItem.h"
#include "MCFrameRenderer.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

#pragma region ctor

	MCRenderItem::MCRenderItem() {
		// Render items can only be created on the main thread
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
	}


	MCRenderItem::~MCRenderItem() { }

#pragma endregion

#pragma region Render

	MC_RESULT XM_CALLCONV MCRenderItem::Render(DirectX::FXMMATRIX viewMatrix, DirectX::CXMMATRIX projMatrix, ID3D12GraphicsCommandList* pCommandList) {

		DirectX::XMMATRIX world = XMLoadFloat4x4(&_worldMatrix);

		DirectX::XMMATRIX worldViewProj = world * viewMatrix*projMatrix;

		for (auto& mesh : _meshes) {
			mesh->Draw(pCommandList);
		}
		return MC_RESULT::OK;
	}

#pragma endregion
}
