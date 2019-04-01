#include "MCREGlobals.h"

namespace MC {

	MCD3D12RenderEngine*          MCREGlobals::pRenderEngine        = nullptr;
	MCD3D*                        MCREGlobals::pMCD3D               = nullptr;
	MCDXGI*                       MCREGlobals::pMCDXGI              = nullptr;
	MCRenderWindow*               MCREGlobals::pRenderWindow        = nullptr;
	MCResourceManager*            MCREGlobals::pResourceManager     = nullptr;
	MCShaderManager*              MCREGlobals::pShaderManager       = nullptr;
	MCRootSignatureManager*       MCREGlobals::pRSManager           = nullptr;
	MCPipelineStateObjectManager* MCREGlobals::pPSOManager          = nullptr;
	RENDER_CONFIG                 MCREGlobals::InitialConfiguration = {};
}