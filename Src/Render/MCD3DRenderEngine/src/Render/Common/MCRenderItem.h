#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Shader/MCShaderManager.h"
#include "../../Core/MCD3D.h"
#include "../../RootSignature/MCRootSignatureManager.h"
#include "../../PipelineStateObject/MCPipelineStateObjectManager.h"

namespace MC {

	class MCRenderItem
	{
	public: /* ctor */
		MCRenderItem(
			std::vector<std::unique_ptr<MCIMesh>> _meshes,
			HShader        hShader,
			HRootSignature hRootSignature,
			HPipelineState hPipelineState
		);

		~MCRenderItem();

		MCRenderItem(MCRenderItem&)             = delete;
		MCRenderItem(MCRenderItem&&)            = delete;
		MCRenderItem& operator=(MCRenderItem&)  = delete;
		MCRenderItem& operator=(MCRenderItem&&) = delete;

	public: /* API */
		MC_RESULT XM_CALLCONV Render(DirectX::FXMMATRIX viewMatrix, DirectX::CXMMATRIX projMatrix, ID3D12GraphicsCommandList* pCommandList);

	private:
		DirectX::XMFLOAT4X4                   _worldMatrix;
		std::vector<std::unique_ptr<MCIMesh>> _meshes;
		HShader                               _hShader;
		HRootSignature						  _hRootSignature;
		HPipelineState						  _hPipelineState;
	};

}

