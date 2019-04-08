#include "MCFrameRendererExecuter.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"


namespace MC {
	
	std::atomic_uint s_nextThreadID{ 0 };

#pragma region ctor

	MCFrameRendererExecuter::MCFrameRendererExecuter(const std::string& name)
		: _name{ name },
		  _executionStage{ MCFRAME_RENDERER_EXECUTION_STAGE_NO_THREAD },
		  _exitFlag{ false },
		  _pThread{nullptr},
		  _pRenderer{nullptr},
		  _pNextFrame{ nullptr },
		  _nextTargetInfo{ 0 } {

		// An executer should only be created on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);		
	}

	MCFrameRendererExecuter::~MCFrameRendererExecuter() {
		// An executer should only be destroyed on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		if (_pRenderer && _pThread)
			DestroyCurrentRenderer();
	}

#pragma endregion

#pragma region Render Control

	MC_RESULT MCFrameRendererExecuter::QueueNextFrame(MCFrame *pFrame, const MCFrameRendererTargetInfo& targetInfo) {

		if (_executionStage.load() != MCFRAME_RENDERER_EXECUTION_STAGE_IDLE)
			return MC_RESULT_FAIL_NOT_READY;

		assert(pFrame);
		assert(!_pNextFrame);

		_pNextFrame.reset(pFrame);
		_nextTargetInfo = targetInfo;

		_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_FRAME_ACCEPTED);

		return MC_RESULT_OK;
	}

	void MCFrameRendererExecuter::NotifyFramePresented() {
		assert(_executionStage.load() == MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT);
		_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_IDLE);
	}

	void MCFrameRendererExecuter::DestroyCurrentRenderer() {
		StopRenderThread();

		if (_pRenderer)
			_pRenderer = nullptr;
	}

	void MCFrameRendererExecuter::SetFrameRenderer(std::unique_ptr<MCFrameRenderer> pRenderer) {
		assert(!_pRenderer);
		assert(!_pThread);

		_pRenderer = std::move(pRenderer);

		StartRenderThread();
	}

#pragma region Thread Control

	void MCFrameRendererExecuter::StartRenderThread() {
		assert(_pRenderer);
		assert(!_pThread);

		RENDER_TRACE("Render Executer {0} is starting thread for renderer {1}", _name, _pRenderer->Name());

		_exitFlag = false;
		_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_IDLE);
		_pThread = std::make_unique<std::thread>(&MCFrameRendererExecuter::RenderLoop, this);
	}

	void MCFrameRendererExecuter::StopRenderThread() {
		assert(_pThread);

		RENDER_TRACE("Renderer Executer {0} is stopping thread for renderer {1}", _name, (_pRenderer ? _pRenderer->Name() : std::string("nullptr")));
		_exitFlag = true;
		_pThread->join();

		// We should always exit the render thread with the next frame having been destroyed.
		assert(!_pNextFrame);

		_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_NO_THREAD);
	}

#pragma endregion

#pragma region Render Loop

	void MCFrameRendererExecuter::RenderLoop() {

#ifdef __MC_THREAD_EXCEPTION_REPORTING__
		try {
#endif __MC_THREAD_EXCEPTION_REPORTING__

		MCThreads::RegisterCurrentThread(
			std::string(_name),
			MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER
		);

		while (!_exitFlag) {
			if (_executionStage.load() == MCFRAME_RENDERER_EXECUTION_STAGE_FRAME_ACCEPTED) {
				
				std::unique_ptr<MCFrame> _pCurrentFrame = std::move(_pNextFrame);
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_CPU_RENDERING);
				auto fenceValue = _pRenderer->RenderFrame(_pCurrentFrame.release(), _nextTargetInfo);
				std::this_thread::sleep_for(std::chrono::milliseconds(MCMath::Rand(1, 15)));
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_GPU_RENDERING);
				std::this_thread::sleep_for(std::chrono::milliseconds(MCMath::Rand(1, 15)));
				MCREGlobals::pMCD3D->WaitForFenceValue(fenceValue);
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT);
			}
		}

		RENDER_INFO("Exiting thread for Render Executer {0}", _name);

#ifdef __MC_THREAD_EXCEPTION_REPORTING__
		}
		catch (MCException *ex) {
			MessageBox(nullptr, ex->what().c_str(), "MCException", MB_OK);
		}
		catch (MCException ex) {
			MessageBox(nullptr, ex.what().c_str(), "MCException", MB_OK);
		}
#endif __MC_THREAD_EXCEPTION_REPORTING__
	}

#pragma endregion
}
