#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"

namespace MC {

	struct RENDER_ENGINE_CONFIGURATION {
		RENDER_CONFIG RenderConfig;
	};

	class MCREGlobals {
		static MCD3D12RenderEngine *pRenderEngine;
		static MCD3D               *pMCD3D;
		static MCDXGI              *pMCDXGI;
		static MCRenderWindow      *pRenderWindow;
		static MCResourceManager   *pResourceManager;
		static MCShaderManager     *pShaderManager;
	};

	class MCD3D12RenderEngine {
	public:
		MCD3D12RenderEngine(const RENDER_ENGINE_CONFIGURATION *pConfiguration);
		~MCD3D12RenderEngine();
		MCD3D12RenderEngine(MCD3D12RenderEngine&)             = delete;
		MCD3D12RenderEngine(MCD3D12RenderEngine&&)            = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&)  = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&&) = delete;
	public:			
	private:
		const RENDER_ENGINE_CONFIGURATION _initialConfiguration;
		private:
			std::unique_ptr<MCD3D>             _pMCD3D;
			std::unique_ptr<MCDXGI>            _pDXGI;
			std::unique_ptr<MCResourceManager> _pResourceManager;
			std::unique_ptr<MCShaderManager>   _pShaderManager;
			std::unique_ptr<MCRenderWindow>    _pRenderWindow;
	};
  
}
