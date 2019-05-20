#include "MCRenderer.h"
#include "assert.h"
#include "Render3D/MCSpaceRenderer/MCSpaceRenderer.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

#pragma region ctor

	MCRenderer::MCRenderer() : _state{ MC_RENDER_STATE_OFF } {
		_pScheduler = std::make_unique<MCFrameScheduler>();
	}

	MCRenderer::~MCRenderer() { }

#pragma endregion

#pragma region update

	void MCRenderer::Update() {
		if (MCREGlobals::pMCDXGI->IsResizeQueued())
			HandleResize();
		_pScheduler->UpdateScheduler();
	}

#pragma endregion

#pragma region handle resize

	void MCRenderer::HandleResize() {
		// Pause the executors
		auto result = _pScheduler->SuspendSync();
		if (result != MC_RESULT::OK) {
			MCTHROW("Error suspending an executer.");
		}

		_pScheduler->OnResizing();

		MCREGlobals::pMCD3D->Resize();

		_pScheduler->ReAcuireRenderTargets();

		_pScheduler->Unsuspend();
	}

#pragma endregion

#pragma region suspend or unsuspend execution

	MC_RESULT MCRenderer::SuspendExecution() {
		if (!_pScheduler)
			return MC_RESULT::FAIL_NO_OBJECT;

		return _pScheduler->SuspendSync();
	}

	MC_RESULT MCRenderer::UnsuspendExecution() {
		if (!_pScheduler)
			return MC_RESULT::FAIL_NO_OBJECT;

		_pScheduler->Unsuspend();
		return MC_RESULT::OK;
	}

#pragma endregion

#pragma region SetState

	MC_RESULT MCRenderer::SetState(MC_RENDER_STATE state) {
		// The state can only be changed from the main thread. This must be enforced along with
		// enforcing operations with the scene loader to happen on the main thread. This means
		// we do not need any synchronization code between setting the render state (which and
		// create and destroy renderers and therefore scenes) and loading items into the current
		// render scene.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		// Don't do anything if we aren't actually changing states.
		if (state == _state)
			return MC_RESULT::OK;

		// If state == MC_RENDER_STATE_OFF shut down the scheduler by
		// removing the renderers.
		else if (state == MC_RENDER_STATE_OFF) {
			// Destroy the renderers currently residing in the scheduler's executers
			_pScheduler->SetRenderers(nullptr, 0);

			// Destroy the current render scene.
			_pScene = nullptr;

			_state = state;
			return MC_RESULT::OK;
		}

		// Give a new renderer to the scheduler, which in turn, will pass it
		// to the executer.
		else if (state == MC_RENDER_STATE_SPACE) {
			static_assert(FRAME_BUFFER_COUNT == 2, "Code is dependent on FRAME_BUFFER_COUNT == 2");

			// setting the state to space requires that the state is currently in 
			// state off.
			if (_state != MC_RENDER_STATE_OFF)
				return MC_RESULT::FAIL_INVALID_STATE;

			// Create a new scene.
			InitializeNewScene();

			// Create the new renderers.
			MCFrameRenderer *ppRenderers[FRAME_BUFFER_COUNT];
			ppRenderers[0] = new MCSpaceRenderer("MCSpaceRenderer 0", 0, _pScene.get());
			ppRenderers[1] = new MCSpaceRenderer("MCSpaceRenderer 1", 1, _pScene.get());

			_pScheduler->SetRenderers(ppRenderers, _countof(ppRenderers));
			_state = state;
			return MC_RESULT::OK;
		}

		else {
			MCTHROW("unknown MC_RENDERER_STATE");
			return MC_RESULT::FAIL_INVALID_DATA;
		}
	}

#pragma endregion

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
		assert(!_pScene);
		_pScene = std::make_unique<MCRenderScene>();
	}

#pragma endregion new scene

}
