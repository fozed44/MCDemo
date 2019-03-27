#include "MCD3DRenderEngine.h"

namespace MC {

  MCD3D12RenderEngine* MCD3D12RenderEngine::pInstance;
  MCD3D* MCD3D12RenderEngine:pMCD3D;
  MCDXGI* MCD3D12RenderEngine::pMCDXGI;
  MCResourceManager* MCD3D12RenderEngine::pResourceManager;
  MCShaderManager*   MCD3D12RenderEngine::pShaderManager;

  MCD3DRenderEngine::MCD3DRenderEngine() {
    pResourceManager = _pResourceManager.Get();
  }

}
