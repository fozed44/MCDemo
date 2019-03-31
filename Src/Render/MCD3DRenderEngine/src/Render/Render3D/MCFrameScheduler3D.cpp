#include "MCFrameScheduler3D.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

	MCFrameScheduler3D::MCFrameScheduler3D() : _nextFrame{ 0 } {
		for (unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x] = std::make_unique<MCFrameRendererExecuter3D>();
	}


	MCFrameScheduler3D::~MCFrameScheduler3D()
	{
	}

	MC_RESULT MCFrameScheduler3D::QueueFrame(MCFrame3D *pFrame) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		MC_RESULT result = _ppExecuters[_nextFrame]->QueueNextFrame(pFrame);

		if (result != MC_RESULT_OK)
			return result;

		_nextFrame = ++_nextFrame % FRAME_BUFFER_COUNT;

		return MC_RESULT_OK;
	}

}