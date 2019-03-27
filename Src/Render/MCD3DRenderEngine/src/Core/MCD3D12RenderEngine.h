#pragma once

#include "D3DCore.h"

namespace MC {

class MCD3D12RenderEngine {
	public:
		~MCD3D12RenderEngine();
		MCD3D12RenderEngine(MCD3D12RenderEngine&)             = delete;
		MCD3D12RenderEngine(MCD3D12RenderEngine&&)            = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&)  = delete;
		MCD3D12RenderEnging& operator=(MCD3D12RenderEngine&&) = delete;
	private:
		MCD3D12RenderEngine();
	public:
		static MCD3D12RenderEngine *pInstance;
		static MCD3D               *pMCD3D;
		static MCDXGI              *pMCDXGI;
		static MCResourceManager   *pResourceManager;
		static MCShaderManager     *pShaderManager;
	private:
		std::unique_ptr<MCD3D>             _pMCD3D;
		std::unique_ptr<MCDXGI>            _pDXGI;
		std::unique_ptr<MCResourceManager> _pResourceManager;
		std::unique_ptr<MCShaderManager>   _pShaderManager;
	}
  
}
