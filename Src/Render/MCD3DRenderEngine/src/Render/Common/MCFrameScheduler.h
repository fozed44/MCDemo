#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../../Common/MCCommon/src/Data/MCFrame.h"
#include "MCFrameRendererExecuter.h"

#include <queue>

namespace MC {

	const int EXECUTER_COUNT = FRAME_BUFFER_COUNT - 1;

	class MCFrameScheduler
	{
	public:
		MCFrameScheduler();
		~MCFrameScheduler();

		MCFrameScheduler(MCFrameScheduler&)             = delete;
		MCFrameScheduler(MCFrameScheduler&&)            = delete;
		MCFrameScheduler& operator=(MCFrameScheduler&)  = delete;
		MCFrameScheduler& operator=(MCFrameScheduler&&) = delete;

	public:

		/*
			Queue the frame to be rendered.

			NOTE:
			
			If QueueFrame returns MC_RESULT_OK then the renderer has taken ownership of the frame memory.
			If, however, QueueFrame3d returns a fail result, such as MC_RESULT_FAIL_NOT_READY, then the caller
			is still responsible for the frame memory.
		*/
		MC_RESULT QueueFrame(void *pFrame);

		MC_RESULT PresentNext();

	private:
		std::unique_ptr<MCFrameRendererExecuter> _ppExecuters[EXECUTER_COUNT];
		std::queue<unsigned int>                 _presentQueue;
		unsigned int                             _nextRenderTargetIndex;
		unsigned int                             _nextDepthBufferIndex;
		
		MCCriticalSectionLock                    _lock;

		void IncrementCounters();

		int GetReadyExecuterIndex();

		void GetRenderTargetInfo(MCFrameRendererTargetInfo *pInfo);
	};

}

