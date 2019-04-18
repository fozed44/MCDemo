#pragma once

#include "../../../../Common/MCCommon/src/Data/MCResult.h"
#include "Common/MCFrameScheduler.h"

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

			    *----------*
			    | Renderer |
			    *----------*
					 |
					 +
			 *----------------*
			 | FrameScheduler |
			 *----------------*
					 |
					 +
		 *-----------------------*
		 | FrameRendererExecuter |
		 *-----------------------*
					 |
					 +
		    *------------------*
		    |   FrameRenderer  |
		    *------------------*
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
		void SetState(MC_RENDER_STATE state);

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

	private:
		MC_RENDER_STATE                     _state;
		std::unique_ptr<MCFrameScheduler>   _pScheduler;
	};

}