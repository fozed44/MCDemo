#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../../Common/MCCommon/src/Data/MCFrame.h"
#include "MCFrameRendererExecuter.h"

#include <queue>

namespace MC {

	const unsigned int EXECUTER_COUNT = FRAME_BUFFER_COUNT - 1;
	const unsigned int FRAME_QUEUE_SIZE = 2;

	class MCFrameScheduler
	{
	public: /* ctor */
		MCFrameScheduler();
		~MCFrameScheduler();

		MCFrameScheduler(MCFrameScheduler&)             = delete;
		MCFrameScheduler(MCFrameScheduler&&)            = delete;
		MCFrameScheduler& operator=(MCFrameScheduler&)  = delete;
		MCFrameScheduler& operator=(MCFrameScheduler&&) = delete;

	public: /* Public methods */

		/*	Schedule the frame to be rendered.

			NOTE:

			If ScheduleFrame returns MC_RESULT_OK then the scheduler has taken ownership of the frame memory.
			If, however, ScheduleFrame returns a fail result, such as MC_RESULT_FAIL_NOT_READY, then the caller
			is still responsible for the frame memory. */
		MC_RESULT ScheduleFrame(MCFrame *pFrame);

		/*	Per-Game-Loop scheduler processing. Should be called by the engine once per game loop iteration. */
		void UpdateSchedule();

		/* Set the renderers used by the executers. */
		void SetRenderers(MCFrameRenderer **pRenderers, unsigned int numRenderers);

		void Kill();

	private: /* Queue and Present. */
		/*	The Queue and Present methods are called by (directly or indirectly) by UpdateSchedule. These are
		the methods that perform the per-game-loop frame scheduling logic. */

		/********************************************************************************/
		
		void TryQueueNextFrame();

		void QueueFrame(MCFrame *pFrame, unsigned int executerIndex);

		void TryPresentNext();

		void IncrementCounters();

		int GetReadyExecuterIndex();

		void GetRenderTargetInfo(MCFrameRendererTargetInfo *pInfo);

	private: /* Private instance data. */
		std::unique_ptr<MCFrameRendererExecuter> _ppExecuters[EXECUTER_COUNT];
		std::queue<unsigned int>                 _presentQueue;
		std::queue<std::unique_ptr<MCFrame>>     _frameQueue;
		unsigned int                             _nextRenderTargetIndex;
		unsigned int                             _nextDepthBufferIndex;
		
		MCCriticalSectionLock                    _lock;

	};

}

