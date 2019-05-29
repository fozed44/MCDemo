#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Shader/MCShaderManager.h"
#include "../../Core/MCD3D.h"
#include "../../RootSignature/MCRootSignatureManager.h"
#include "../../PipelineStateObject/MCPipelineStateObjectManager.h"
#include "../../Core/MCRenderObjects.h"

namespace MC {

	class MCRenderItem
	{
	public: /* ctor */
		MCRenderItem(
			std::unique_ptr<MCIMesh> pMeshes,
			HShader                  hShader,
			HRootSignature           hRootSignature,
			HPipelineState           hPipelineState
		);

		~MCRenderItem();

		MCRenderItem(MCRenderItem&)             = delete;
		MCRenderItem(MCRenderItem&&)            = delete;
		MCRenderItem& operator=(MCRenderItem&)  = delete;
		MCRenderItem& operator=(MCRenderItem&&) = delete;

	public: /* API */
		MC_RESULT XM_CALLCONV Render(
			DirectX::FXMMATRIX         viewMatrix,
			DirectX::CXMMATRIX         projMatrix,
			DirectX::CXMMATRIX         worldMatrix,
			ID3D12GraphicsCommandList* pCommandList
		);

	private:
		std::vector<std::unique_ptr<MCIMesh>> _meshes;
		HShader                               _hShader;
		HRootSignature						  _hRootSignature;
		HPipelineState						  _hPipelineState;
		HResource                             _hObjectConstantBuffer;
	};

}

