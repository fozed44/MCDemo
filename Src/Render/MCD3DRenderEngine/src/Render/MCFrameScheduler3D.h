#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"
#include "../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../Common/MCCommon/src/Data/MCFrame.h"
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
		MC_RESULT Update(float gameTime);

		/*
			Queue the frame to be rendered.

			NOTE:
			
			If QueueFrame3D returns MC_RESULT_OK then the renderer has taken owner ship of the frame memory.
			If, however, QueueFrame3d returns an fail result such as MC_RESULT_FAIL_NOT_READY then the caller
			is still responsible for the frame memory.
		*/
		MC_RESULT QueueFrame3D(MCFrame3D *pFrame);
		MC_RESULT QueueFrame2D(MCFrame2D *pFrame);

	private:
		std::unique_ptr<MCFrameRendererExecuter3D> _ppExecuters[FRAME_BUFFER_COUNT];
		std::queue<MCFrame3D> _frameQueue3D;
		std::queue<MCFrame2D> _frameQueue2D;
	};

}

