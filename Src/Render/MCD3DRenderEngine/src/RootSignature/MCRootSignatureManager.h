#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	typedef enum class STANDARD_ROOT_SIGNATURE {
		DEFAULT = 0
	} STANDARD_ROOT_SIGNATURE;

	class MCRootSignatureManager : public MCManagedHandleManager<ID3D12RootSignature*, ComPtr<ID3D12RootSignature>, MCRootSignatureManager>  {
	public:
		using HRootSignature = MCRootSignatureManager::HandleType;
	public: /* ctor */
		MCRootSignatureManager();
		~MCRootSignatureManager();
		MCRootSignatureManager(MCRootSignatureManager&)              = delete;
		MCRootSignatureManager(MCRootSignatureManager&&)             = delete;
		MCRootSignatureManager& operator= (MCRootSignatureManager&)  = delete;
		MCRootSignatureManager& operator= (MCRootSignatureManager&&) = delete;

	public: /* Instance access */
		static MCRootSignatureManager* Instance() noexcept { return MCREGlobals::pRSManager; }

	public: /* API */

		/*	Create a NEW root signature. Most of the time client code should be using a standard root signature
		  rather than creating a new one. */
		HRootSignature CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, const std::string& name);

		/*	Converts a managed handle to a root signature to the underlying ID3D12RootSignature* */
		inline ID3D12RootSignature* GetRootSignature(const HRootSignature& hSig) const noexcept { return UnwrapHandle(hSig); }

		/*	Use GetRootSignatureHandle to get a signature handler to a standard (built in) signature. */
		inline HRootSignature GetRootSignatureHandle(STANDARD_ROOT_SIGNATURE standardSignature);

	private: /* Standard Root Signatures */
		std::map<STANDARD_ROOT_SIGNATURE, ComPtr<ID3D12RootSignature>> _standardSignatures;

		void InitializeStandardRootSignatures();
	};

	using HRootSignature = MCRootSignatureManager::HRootSignature;
}