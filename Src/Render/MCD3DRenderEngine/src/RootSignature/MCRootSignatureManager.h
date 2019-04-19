#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {

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

		HRootSignature CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, const std::string& name);
		inline const ID3D12RootSignature* GetRootSignature(const HRootSignature& hSig) const noexcept { return UnwrapHandle(hSig); }

	private: /* name */
		std::map<std::string, ID3D12RootSignature*> _shaderMap;
	};

	using HRootSignature = MCRootSignatureManager::HRootSignature;
}