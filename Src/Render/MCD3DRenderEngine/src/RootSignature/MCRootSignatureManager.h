#pragma once

#include "../Core/MCREGlobals.h"

namespace MC {

	class MCRootSignatureManager {
	public: /* ctor */
		MCRootSignatureManager();
		~MCRootSignatureManager();
		MCRootSignatureManager(MCRootSignatureManager&)              = delete;
		MCRootSignatureManager(MCRootSignatureManager&&)             = delete;
		MCRootSignatureManager& operator= (MCRootSignatureManager&)  = delete;
		MCRootSignatureManager& operator= (MCRootSignatureManager&&) = delete;

	public: /* Instance access */
		static MCRootSignatureManager* Instance() {
			return MCREGlobals::pRSManager;
		}
	};

}