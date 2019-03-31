#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../../Common/MCCommon/src/Data/MCFrame.h"
#include "MCFrameRendererExecuter3D.h"

#include <queue>

namespace MC {

	const int MAX_RENDER_MANAGER_QUEUE_SIZE = 5;

	class MCFrameScheduler3D
	{
	public:
		MCFrameScheduler3D();
		~MCFrameScheduler3D();

		MCFrameScheduler3D(MCFrameScheduler3D&)             = delete;
		MCFrameScheduler3D(MCFrameScheduler3D&&)            = delete;
		MCFrameScheduler3D& operator=(MCFrameScheduler3D&)  = delete;
		MCFrameScheduler3D& operator=(MCFrameScheduler3D&&) = delete;

	public:

		/*
			Queue the frame to be rendered.

			NOTE:
			
			If QueueFrame returns MC_RESULT_OK then the renderer has taken ownership of the frame memory.
			If, however, QueueFrame3d returns an fail result such as MC_RESULT_FAIL_NOT_READY then the caller
			is still responsible for the frame memory.
		*/
		MC_RESULT QueueFrame(MCFrame3D *pFrame);

	private:
		std::unique_ptr<MCFrameRendererExecuter3D> _ppExecuters[FRAME_BUFFER_COUNT];
		unsigned int							   _nextFrame;
	};

}

