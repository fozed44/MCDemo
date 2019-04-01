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

}