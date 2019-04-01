#pragma once

#include "../Core/MCREGlobals.h"

namespace MC {

	class MCPipelineStateObjectManager {
	public: /* ctor */
		MCPipelineStateObjectManager();
		~MCPipelineStateObjectManager();
		MCPipelineStateObjectManager(MCPipelineStateObjectManager&) = delete;
		MCPipelineStateObjectManager(MCPipelineStateObjectManager&&) = delete;
		MCPipelineStateObjectManager& operator= (MCPipelineStateObjectManager&) = delete;
		MCPipelineStateObjectManager& operator= (MCPipelineStateObjectManager&&) = delete;

	public: /* Instance access */
		static MCPipelineStateObjectManager* Instance() {
			return MCREGlobals::pPSOManager;
		}
	};
}