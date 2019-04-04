#include "MCRenderer.h"
#include "assert.h"
#include "Render3D/MCSpaceRenderer/MCSpaceRenderer.h"

namespace MC {

	MCRenderer::MCRenderer() : _state{ MC_RENDERER_STATE_OFF } { }

	MCRenderer::~MCRenderer() { }

	void MCRenderer::Update() {
		_pScheduler->Update();
	}

	void MCRenderer::SetState(MC_RENDERER_STATE state) {
		// Don't do anything if we aren't actually changing states.
		if (state == _state)
			return;

		// Stop the scheduler from rendering any more on the current
		// frame renderer(s).
		_pScheduler->Kill();

		// Give new renderers to the scheduler, which in turn, will pass them
		// to the executer.
		MCFrameRenderer *pRenderers[FRAME_BUFFER_COUNT - 1];
		if (state == MC_RENDERER_STATE_SPACE) {
			for (int x = 0; x < FRAME_BUFFER_COUNT - 1; ++x)
				pRenderers[x] = new MCSpaceRenderer;
			_pScheduler->SetRenderers()
		}
	}

	MC_RESULT MCRenderer::QueueFrame(void *pFrame) {
		if (_state == MC_RENDERER_STATE_OFF)
			return MC_RESULT_FAIL_OFF;
		if (_state == MC_RENDERER_STATE_SPACE) {
			return _scheduler->QueueFrame(ptr);
		}
		return MC_RESULT_ERROR;
	}

}
