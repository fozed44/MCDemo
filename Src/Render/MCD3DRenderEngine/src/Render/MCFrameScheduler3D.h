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
		MC_RESULT QueueFrame3D(const MCFrame3D& frame);
		MC_RESULT QueueFrame2D(const MCFrame2D& frame);

	private:
		MCFrameRendererExecuter3D _pFrameRenderers[FRAME_BUFFER_COUNT];
		std::queue<MCFrame3D> _frameQueue3D;
		std::queue<MCFrame2D> _frameQueue2D;
	};

}

