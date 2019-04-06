#include "MCRenderer.h"
#include "assert.h"
#include "Render3D/MCSpaceRenderer/MCSpaceRenderer.h"

namespace MC {

	MCRenderer::MCRenderer() : _state{ MC_RENDER_STATE_OFF } {
		_pScheduler = std::make_unique<MCFrameScheduler>();
	}

	MCRenderer::~MCRenderer() { }

	void MCRenderer::Update() {
		_pScheduler->UpdateSchedule();
	}

	void MCRenderer::SetState(MC_RENDER_STATE state) {
		// Don't do anything if we aren't actually changing states.
		if (state == _state)
			return;

		// If state == MC_RENDER_STATE_OFF shut down the scheduler by
		// removing the renderers.
		if (state == MC_RENDER_STATE_OFF) {
			_pScheduler->SetRenderers(nullptr, 0);
			_state = state;
			return;
		}

		// Give new renderers to the scheduler, which in turn, will pass them
		// to the executer.
		MCFrameRenderer *pRenderers[FRAME_BUFFER_COUNT - 1];
		if (state == MC_RENDER_STATE_SPACE) {
			for (int x = 0; x < FRAME_BUFFER_COUNT - 1; ++x)
				pRenderers[x] = new MCSpaceRenderer(std::string("MCSpaceRenderer ") + std::to_string(x));
			_pScheduler->SetRenderers(pRenderers, _countof(pRenderers));
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
