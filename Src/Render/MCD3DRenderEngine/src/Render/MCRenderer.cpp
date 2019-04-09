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
			_pScheduler->SetRenderers(nullptr,0);
			_state = state;
			return;
		}

		// Give a new renderer to the scheduler, which in turn, will pass it
		// to the executer.
		if (state == MC_RENDER_STATE_SPACE) {
			static_assert(FRAME_BUFFER_COUNT == 2, "Code is dependent on FRAME_BUFFER_COUNT == 2");

			MCFrameRenderer *ppRenderers[FRAME_BUFFER_COUNT];
			ppRenderers[0] = new MCSpaceRenderer("MCSpaceRenderer 0", 0);
			ppRenderers[1] = new MCSpaceRenderer("MCSpaceRenderer 1", 1);

			_pScheduler->SetRenderers(ppRenderers, _countof(ppRenderers));
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
