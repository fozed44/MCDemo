#include "MCRenderer.h"
#include "assert.h"
#include "Render3D/MCSpaceRenderer/MCSpaceRenderer.h"

namespace MC {

	MCRenderer::MCRenderer() : _state{ MC_RENDER_STATE_OFF } {
		_pScheduler = std::make_unique<MCFrameScheduler>();
	}

	MCRenderer::~MCRenderer() { }

	void MCRenderer::Update() {
		_pScheduler->UpdateScheduler();
	}

	void MCRenderer::SetState(MC_RENDER_STATE state) {
		// Don't do anything if we aren't actually changing states.
		if (state == _state)
			return;

		// If state == MC_RENDER_STATE_OFF shut down the scheduler by
		// removing the renderers.
		if (state == MC_RENDER_STATE_OFF) {
			_pScheduler->SetRenderer(nullptr);
			_state = state;
			return;
		}

		// Give a new renderer to the scheduler, which in turn, will pass it
		// to the executer.
		if (state == MC_RENDER_STATE_SPACE) {
			_pScheduler->SetRenderer(new MCSpaceRenderer("Space Renderer"));
			_state = state;
			return;
		}
		MCTHROW("unknown MC_RENDERER_STATE");
	}

	MC_RESULT MCRenderer::ScheduleFrame(MCFrame *pFrame) {
		if (_state == MC_RENDER_STATE_OFF)
			return MC_RESULT_FAIL_OFF;

		return _pScheduler->ScheduleFrame(pFrame);
	}

}
