#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../Core/MCD3D.h"
#include "MCRootSignatureManager.h"
#include "MCRootSignatures.h"
#include "assert.h"

namespace MC {

#pragma region ctor

	MCRootSignatureManager::MCRootSignatureManager() {
		// ensure this object is only created a single time.
		assert(!MCREGlobals::pRSManager);

		// Ensure that this object is only created on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		// Create the standard root signatures
		InitializeStandardRootSignatures();
	}

	MCRootSignatureManager::~MCRootSignatureManager() {}

#pragma endregion

#pragma region create signatures

	MCRootSignatureManager::HRootSignature MCRootSignatureManager::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, const std::string& name) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

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

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateRootSignature(
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

#pragma region Standard Signatures

	void MCRootSignatureManager::InitializeStandardRootSignatures() {
		// Create the default root signature.
		_standardSignatures[STANDARD_ROOT_SIGNATURE::DEFAULT] = MC::RootSignatures::Default();
	}

	HRootSignature MCRootSignatureManager::GetRootSignatureHandle(STANDARD_ROOT_SIGNATURE standardSignature) {
		auto& comPtr = _standardSignatures[standardSignature];
		return CreateRef(comPtr.Get(), comPtr);
	}

#pragma endregion
}