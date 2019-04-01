#pragma once

#include "../Configuration/RenderConfig.h"

namespace MC {

	class MCD3D12RenderEngine;
	class MCD3D;
	class MCDXGI;
	class MCRenderWindow;
	class MCResourceManager;
	class MCShaderManager;
	class MCPipelineStateObjectManager;
	class MCRootSignatureManager;
	

	/*
		MCREGlobals

		This class has no methods or instance properties. It is strictly a container for the various 'singleton' objects.
		'singleton' is quoted because these objects are not true singletons. MCD3D12RenderIngine is the 'root' object,
		and must be created normally. The rest of the objects are owned by the render engine. When the render
		engine is created, the render engine will then create these objects which it owns. Once these owned objects
		have been created the constructor of the render engine will then populate these static variables.

		NOTE:
		
		The reason that these 'singleton' objects are owned by the render engine rather than created in true singleton
		fashion is that there are too many singletons in to many translation units, preventing a known order of destruction.
	*/

	struct MCREGlobals {
		static MCD3D12RenderEngine          *pRenderEngine;
		static MCD3D                        *pMCD3D;
		static MCDXGI                       *pMCDXGI;
		static MCRenderWindow               *pRenderWindow;
		static MCResourceManager            *pResourceManager;
		static MCShaderManager              *pShaderManager;
		static MCPipelineStateObjectManager *pPSOManager;
		static MCRootSignatureManager       *pRSManager;
		static RENDER_CONFIG        InitialConfiguration;
	};
}
