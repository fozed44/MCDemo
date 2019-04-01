#include "MCPipelineStateObjectManager.h"
#include "assert.h"

namespace MC {

#pragma region ctor

	MCPipelineStateObjectManager::MCPipelineStateObjectManager() {
		// Ensure a single object instance.
		assert(!MCREGlobals::pPSOManager);
	}

	MCPipelineStateObjectManager::~MCPipelineStateObjectManager() {}

#pragma endregion

}