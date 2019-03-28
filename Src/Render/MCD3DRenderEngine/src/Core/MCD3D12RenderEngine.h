#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"
#include "../Configuration/RenderConfig.h"
#include "MCRenderWindow.h"

namespace MC {

	class MCD3D12RenderEngine {
	public:
		MCD3D12RenderEngine(const RENDER_CONFIG *pRenderConfig);
		~MCD3D12RenderEngine() {}
		MCD3D12RenderEngine(MCD3D12RenderEngine&)             = delete;
		MCD3D12RenderEngine(MCD3D12RenderEngine&&)            = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&)  = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&&) = delete;
	private:
		private:
			std::unique_ptr<MCRenderWindow>    _pRenderWindow;
			std::unique_ptr<MCResourceManager> _pResourceManager;
			std::unique_ptr<MCShaderManager>   _pShaderManager;
			std::unique_ptr<MCDXGI>            _pMCDXGI;
			std::unique_ptr<MCD3D>             _pMCD3D;
	};
  
}
