#pragma once

#include "../../../../Common/MCCommon/src/Data/MCResult.h"
#include "Common/MCFrameScheduler.h"
#include "Common/MCRenderScene.h"

#include <memory>

namespace MC {

	/*

		 The rendering sub-system is divided into four main areas, each implemented by its own class or classes.

		1) MCRenderer:
			MCRenderer is the facade that is provided to MCD3DRenderEngine that enables actual rendering.

		2) MCFrameScheduler:
			Manages a queue of MCFrameData and the timing needed to transfer that data to the Executer.

		3) MCFrameRendererExecuter:
			Manages the thread running the current frame renderer.

		4) MCFrameRenderer:
			One of several implementations of the MCFrameRenderer base class.

			    +----------+
			    | Renderer |----------------+
			    +----------+                 \
					 |                       |
					 +                       |
			 +----------------+              |
			 | FrameScheduler |              |
			 +----------------+              |
					 |                       |
					 +                       |
		 +-----------------------+           |
		 | FrameRendererExecuter |           |
		 +-----------------------+           |
					 |                       |
					 +                       +
		    +------------------+     +-----------------+      
		    |   FrameRenderer  |+--->|  MCRenderScene  |
		    +------------------+     +-----------------+
	*/
	typedef enum MC_RENDER_STATE {
		MC_RENDER_STATE_OFF   = 0,
		MC_RENDER_STATE_SPACE = 1,
		MC_RENDER_STATE_2D    = 2
	} MC_RENDER_STATE;

	class MCRenderer
	{
	public: /* ctor */
		MCRenderer();
		~MCRenderer();
		MCRenderer(MCRenderer&)              = delete;
		MCRenderer(MCRenderer&&)             = delete;
		MCRenderer& operator= (MCRenderer&)  = delete;
		MCRenderer& operator= (MCRenderer&&) = delete;

	public:
		inline MC_RENDER_STATE GetState() const { return _state; }
		MC_RESULT SetState(MC_RENDER_STATE state);

		/* Call Update once per game loop iteration. */
		void Update();

		/*
			Queue the frame to be rendered.

			NOTE:

			If ScheduleFrame returns MC_RESULT::OK then the renderer has taken ownership of the frame memory.
			If, however, ScheduleFrame returns an fail result such as MC_RESULT_FAIL_NOT_READY then the caller
			is still responsible for the frame memory.
		*/
		MC_RESULT ScheduleFrame(MCFrame *pFrame);

		/*	Retrieve a pointer to the current scene loader. 
			
			Note that even though this method it's self does not have any thread restrictions, all of the operations
			on the scene loader are restricted to the main thread in order to prevent interaction with render state
			changes. */
		const MCSceneLoader* GetSceneLoader() { return _pScene->GetSceneLoader(); }

		/* Suspend or UnSuspend render execution. */
		MC_RESULT SuspendExecution();
		MC_RESULT UnsuspendExecution();

	private:
		MC_RENDER_STATE                   _state;
		std::unique_ptr<MCFrameScheduler> _pScheduler;
		std::unique_ptr<MCRenderScene>    _pScene;

	private:
		/*	Called as a consequence of setting a new state. If the new state requires a new renderer
			or renderers then a new scene should be re-created as well. */
		void InitializeNewScene();

		/* Called via the update method. This method is called when a resize is required. */
		void HandleResize();
	};

}