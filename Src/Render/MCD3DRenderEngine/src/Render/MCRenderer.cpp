#include "MCRenderer.h"
#include "assert.h"
#include "Render3D/MCSpaceRenderer/MCSpaceRenderer.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

	MCRenderer::MCRenderer() : _state{ MC_RENDER_STATE_OFF } {
		_pScene     = std::make_unique<MCRenderScene>();
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

			// Create a new scene.
			InitializeNewScene();

			// Create the new renderers.
			MCFrameRenderer *ppRenderers[FRAME_BUFFER_COUNT];
			ppRenderers[0] = new MCSpaceRenderer("MCSpaceRenderer 0", 0, _pScene.get());
			ppRenderers[1] = new MCSpaceRenderer("MCSpaceRenderer 1", 1, _pScene.get());

			_pScheduler->SetRenderers(ppRenderers, _countof(ppRenderers));
			_state = state;
			return;
		}
		MCTHROW("unknown MC_RENDERER_STATE");
	}

	MC_RESULT MCRenderer::ScheduleFrame(MCFrame *pFrame) {
		if (_state == MC_RENDER_STATE_OFF)
			return MC_RESULT::FAIL_OFF;

		return _pScheduler->ScheduleFrame(pFrame);
	}

#pragma region new scene

	/* ATM this method doesn't do much, but in the future it may be required to lock
	   the creation of a new scene, so we put the code in its own method for future
	   proofing. */
	void MCRenderer::InitializeNewScene() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		_pScene = std::make_unique<MCRenderScene>();
	}

#pragma endregion new scene

}
