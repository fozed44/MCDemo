#include "MCRenderer.h"
#include "assert.h"

namespace MC {

	MCRenderer::MCRenderer() : _state{ MC_RENDERER_STATE_OFF } { }

	MCRenderer::~MCRenderer() { }

	MC_RESULT MCRenderer::QueueFrame(void *pFrame) {
		if (_state == MC_RENDERER_STATE_OFF)
			return MC_RESULT_FAIL_OFF;
		if (_state == MC_RENDERER_STATE_3D) {
			MCFrame3D *ptr = static_cast<MCFrame3D*>(pFrame);

			assert(ptr->FrameType == MC_FRAME_TYPE_MCFRAME_3D);

			return _scheduler3D->QueueFrame(ptr);
		}
		return MC_RESULT_ERROR;
	}

}
