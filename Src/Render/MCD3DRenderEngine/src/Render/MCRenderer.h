#pragma once

#include "../../../../Common/MCCommon/src/Data/MCResult.h"
#include "Common/MCFrameScheduler.h"

#include <memory>

namespace MC {

	typedef enum MC_RENDERER_STATE {
		MC_RENDERER_STATE_OFF   = 0,
		MC_RENDERER_STATE_SPACE = 1,
		MC_RENDERER_STATE_2D    = 2
	} MC_RENDERER_STATE;

	class MCRenderer
	{
	public:
		MCRenderer();
		~MCRenderer();
		MCRenderer(MCRenderer&)              = delete;
		MCRenderer(MCRenderer&&)             = delete;
		MCRenderer& operator= (MCRenderer&)  = delete;
		MCRenderer& operator= (MCRenderer&&) = delete;

	public:
		inline MC_RENDERER_STATE GetState() const { return _state; }
		inline void SetState(MC_RENDERER_STATE state);

		/* Call Update once per game loop iteration. */
		void Update();

		/*
			Queue the frame to be rendered.

			NOTE:

			If QueueFrame returns MC_RESULT_OK then the renderer has taken ownership of the frame memory.
			If, however, QueueFrame3d returns an fail result such as MC_RESULT_FAIL_NOT_READY then the caller
			is still responsible for the frame memory.
		*/
		MC_RESULT QueueFrame(void *pFrame);

	private:
		MC_RENDERER_STATE                   _state;
		std::unique_ptr<MCFrameScheduler>   _pScheduler;
	};

}