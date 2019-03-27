#include "MCD3D12RenderEngine.h"

namespace MC {

  MCD3D12RenderEngine* MCD3D12RenderEngine::pInstance;
  MCD3D* MCD3D12RenderEngine:pMCD3D;
  MCDXGI* MCD3D12RenderEngine::pMCDXGI;
  MCResourceManager* MCD3D12RenderEngine::pResourceManager;
  MCShaderManager*   MCD3D12RenderEngine::pShaderManager;

  MCD3D12RenderEngine::MCD3D12RenderEngine(const RENDER_ENGINE_CONFIGURATION *pConfiguration)
	  : _initialConfiguration{ *pConfiguration } {}

}
