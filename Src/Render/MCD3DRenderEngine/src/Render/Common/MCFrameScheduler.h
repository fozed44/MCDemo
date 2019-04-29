#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../../Common/MCCommon/src/Data/MCFrame.h"
#include "MCFrameRendererExecuter.h"

#include <queue>

namespace MC {

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

			If ScheduleFrame returns MC_RESULT::OK then the scheduler has taken ownership of the frame memory.
			If, however, ScheduleFrame returns a fail result, such as MC_RESULT_FAIL_NOT_READY, then the caller
			is still responsible for the frame memory. */
		MC_RESULT ScheduleFrame(MCFrame *pFrame);

		/*	Per-Game-Loop scheduler processing. Should be called by the engine once per game loop iteration. */
		void UpdateScheduler();

		/* Set the renderers used by the executers. 
			NOTE:
				SetRenderer can be called with nullptr as the parameters to safely shut down the current executer. */
		void SetRenderers(MCFrameRenderer **ppRenderers, unsigned int count);

		/* Suspend and Unsuspend the execution of the renderers. These methods must be called from the main thread. */
		MC_RESULT SuspendSync();
		void Unsuspend();

		/* Call to force the renderers to re-acquire their render targets. The executers must be paused. */
		void ReAcuireRenderTargets();

	private: /* Queue and Present. */
		/*	The Queue and Present methods are called by (directly or indirectly) by UpdateSchedule. These are
		the methods that perform the per-game-loop frame scheduling logic. */

		/********************************************************************************/
		
		void TryQueueNextFrame();

		void QueueFrame(MCFrame *pFrame);

		void IncrementNextExecuterIndex();

		void SafeReleaseCurrentRenderers();

	private: /* Private instance data. */
		std::unique_ptr<MCFrameRendererExecuter> _ppExecuters[FRAME_BUFFER_COUNT];
		std::queue<std::unique_ptr<MCFrame>>     _frameQueue;

		unsigned int                             _nextExecuterIndex;
		
		MCCriticalSectionLock                    _lock;

	};

}

