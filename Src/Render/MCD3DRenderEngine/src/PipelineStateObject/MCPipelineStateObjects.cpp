#include "MCPipelineStateObjects.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../Core/MCREGlobals.h"
#include "../RootSignature/MCRootSignatureManager.h"
#include "../Core/MCRenderObjects.h"

namespace MC { namespace PSOs {

	ComPtr<ID3D12PipelineState> Default() {

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

		psoDesc.InputLayout = { MCVertex1Color_desc, _countof(MCVertex1Color_desc) };
		psoDesc.pRootSignature = standardRootSignature;
		psoDesc.VS = MCREGlobals::pShaderManager->GetByteCode
		
	}

} }
