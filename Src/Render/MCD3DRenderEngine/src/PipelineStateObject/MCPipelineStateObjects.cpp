#include "MCPipelineStateObjects.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../Core/MCREGlobals.h"
#include "../RootSignature/MCRootSignatureManager.h"
#include "../Core/MCRenderObjects.h"
#include "../Shader/MCShaderManager.h"
#include "../Core/MCD3D.h"

namespace MC { namespace PSOs {

	ComPtr<ID3D12PipelineState> Default() {
		INIT_TRACE("Create default PSO");

		D3D12_RASTERIZER_DESC rasterizerDesc{};
		
		rasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS; 
		rasterizerDesc.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesc.DepthClipEnable       = TRUE;
		rasterizerDesc.MultisampleEnable     = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.ForcedSampleCount     = 0;
		rasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		auto standardRootSigHandle = MCREGlobals::pRSManager->GetRootSignatureHandle(STANDARD_ROOT_SIGNATURE::DEFAULT);
		auto standardRootSignature = MCREGlobals::pRSManager->GetRootSignature(standardRootSigHandle);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(psoDesc));

		psoDesc.InputLayout           = { MCVertex1Color_desc, _countof(MCVertex1Color_desc) };
		psoDesc.pRootSignature        = standardRootSignature;
		psoDesc.VS                    = MCREGlobals::pShaderManager->GetByteCode(STANDARD_SHADER::DEFAULT_VERTEX);
		psoDesc.PS                    = MCREGlobals::pShaderManager->GetByteCode(STANDARD_SHADER::DEFAULT_PIXEL);
		psoDesc.RasterizerState       = rasterizerDesc;
		psoDesc.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask            = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets      = 1;
		psoDesc.RTVFormats[0]         = BACK_BUFFER_FORMAT;

		psoDesc.SampleDesc.Count   = 1;
		psoDesc.SampleDesc.Quality = 0;

		psoDesc.DSVFormat = MC_DEPTH_STENCIL_FORMAT;

		ComPtr<ID3D12PipelineState> pPipelineState;

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateGraphicsPipelineState(
			&psoDesc,
			__uuidof(pPipelineState),
			&pPipelineState
		));

		pPipelineState->SetName(L"Default PSO");

		INIT_TRACE("Created default PSO.");

		return pPipelineState;
	}

} }
