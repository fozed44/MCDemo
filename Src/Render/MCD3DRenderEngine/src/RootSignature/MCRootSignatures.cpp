#include "MCRootSignatures.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../Core/MCDXGI.h"
#include "../Core/MCREGlobals.h"

namespace MC { namespace RootSignatures {

	ComPtr<ID3D12RootSignature> Default() {
		D3D12_ROOT_SIGNATURE_DESC desc;

		D3D12_ROOT_PARAMETER pSlotRootParameter[1];

		D3D12_DESCRIPTOR_RANGE constantBufferViewTable;

		constantBufferViewTable.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		constantBufferViewTable.NumDescriptors                    = 1;
		constantBufferViewTable.OffsetInDescriptorsFromTableStart = 0;
		constantBufferViewTable.RegisterSpace					  = 0;
		constantBufferViewTable.BaseShaderRegister                = 0;

		pSlotRootParameter[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pSlotRootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
		pSlotRootParameter[0].DescriptorTable.pDescriptorRanges   = &constantBufferViewTable;
		pSlotRootParameter[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

		desc.NumParameters     = 1;
		desc.NumStaticSamplers = 0;
		desc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		desc.pParameters       = pSlotRootParameter;
		desc.pStaticSamplers   = nullptr;

		ComPtr<ID3DBlob> serializedRootSignature = nullptr;
		ComPtr<ID3DBlob> errorBlob               = nullptr;
		MCThrowIfFailed(D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSignature,
			&errorBlob
		));
		
		ComPtr<ID3D12RootSignature> pResult;

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateRootSignature(
			0,
			serializedRootSignature->GetBufferPointer(),
			serializedRootSignature->GetBufferSize(),
			__uuidof(pResult),
			&pResult
		));

		pResult->SetName(L"Default RootSignature.");

		return pResult;
	}
} }