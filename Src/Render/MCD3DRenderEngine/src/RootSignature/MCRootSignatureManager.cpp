#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../Core/MCD3D.h"
#include "MCRootSignatureManager.h"
#include "assert.h"

namespace MC {

#pragma region ctor

	MCRootSignatureManager::MCRootSignatureManager() {
		// ensure this object is only created a single time.
		assert(!MCREGlobals::pRSManager);
	}

	MCRootSignatureManager::~MCRootSignatureManager() {}

#pragma endregion

#pragma region create signatures

	MCRootSignatureManager::HRootSignature MCRootSignatureManager::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, const std::string& name) {
		RENDER_TRACE("Begin initialize shader: {0}", name);
		
		// Create a root signature with a single slot which pointer to a
		// descriptor range consisting of a single constant buffer.
		ComPtr<ID3DBlob> serializedRootSignature = nullptr;
		ComPtr<ID3DBlob> errorBlob               = nullptr;
		MCThrowIfFailed(D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSignature,
			&errorBlob
		));
		
		ComPtr<ID3D12RootSignature> pResult;

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice->CreateRootSignature(
			0,
			serializedRootSignature->GetBufferPointer(),
			serializedRootSignature->GetBufferSize(),
			__uuidof(pResult),
			&pResult
		));
		
		pResult->SetName(s2ws(name).c_str());		

		RENDER_TRACE("End initialize shader: {0}", name);

		return CreateRef(pResult.Get(), pResult);
	}

#pragma endregion

}