#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"
#include "../Configuration/RenderConfig.h"
#include "MCRenderWindow.h"
#include "../Render/MCRenderer.h"
#include "../../../../Common/MCCommon/src/Data/MCObjects.h"
#include "../../../../Common/MCCommon/src/Data/MCFrame.h"

namespace MC {

	class MCD3D12RenderEngine {
	public:
		MCD3D12RenderEngine(const RENDER_CONFIG *pRenderConfig);
		~MCD3D12RenderEngine();
		MCD3D12RenderEngine(MCD3D12RenderEngine&)             = delete;
		MCD3D12RenderEngine(MCD3D12RenderEngine&&)            = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&)  = delete;
		MCD3D12RenderEngine& operator=(MCD3D12RenderEngine&&) = delete;

	public: /* Public API */

		MCHANDLE LoadTexture(MCTEXTURE_DESC desc);

		MCHANDLE LoadMesh(MCMESH_DESC desc);

		MCHANDLE LoadSprite(MCSPRITE_DESC desc);

		/* Call update once per game loop iteration.*/
		void      Update();

		/* Queue a frame to the render engine.

			Note:
				If the result of ScheduleFrame is MC_RESULT::OK then the render engine has taken
				control of the memory for the frame. If the result is a fail result, then the
				caller is still responsible for the memory. */
		MC_RESULT ScheduleFrame(MCFrame *pFrame);

		/* Set the render state. */
		void SetRenderState(MC_RENDER_STATE renderState);
	private: /* Private Members */
		std::unique_ptr<MCRenderer>                   _pRenderer;
	private: /* Instances that will be copied to MCREGlobals */
		std::unique_ptr<MCRenderWindow>               _pRenderWindow;
		std::unique_ptr<MCResourceManager>            _pResourceManager;
		std::unique_ptr<MCShaderManager>              _pShaderManager;
		std::unique_ptr<MCPipelineStateObjectManager> _pPSOManager;
		std::unique_ptr<MCRootSignatureManager>       _pRSManager;
		std::unique_ptr<MCDXGI>                       _pMCDXGI;
		std::unique_ptr<MCD3D>                        _pMCD3D;
	};
  
}
