#include "MCD3D12RenderEngine.h"
#include "MCREGlobals.h"

namespace MC {

  MCD3D12RenderEngine::MCD3D12RenderEngine(const RENDER_CONFIG *pConfiguration) {
	  assert(!MCREGlobals::pRenderEngine);

	  MCREGlobals::InitialConfiguration = *pConfiguration;

	  _pRenderWindow = std::make_unique<MCRenderWindow>();
	  MCREGlobals::pRenderWindow = _pRenderWindow.get();

	  _pMCDXGI = std::make_unique<MCDXGI>();
	  MCREGlobals::pMCDXGI = _pMCDXGI.get();

	  _pMCD3D = std::make_unique<MCD3D>();
	  MCREGlobals::pMCD3D = _pMCD3D.get();

	  _pResourceManager = std::make_unique<MCResourceManager>();
	  MCREGlobals::pResourceManager = _pResourceManager.get();

	  _pShaderManager = std::make_unique<MCShaderManager>();
	  MCREGlobals::pShaderManager = _pShaderManager.get();

	  _pMCD3D->InitTest();

	  MCREGlobals::pRenderEngine = this;
  }

}
