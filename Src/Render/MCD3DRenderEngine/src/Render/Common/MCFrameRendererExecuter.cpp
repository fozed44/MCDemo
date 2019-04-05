#include "MCFrameRendererExecuter.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"


namespace MC {

	std::atomic_uint s_nextThreadID{ 0 };

	MCFrameRendererExecuter::MCFrameRendererExecuter(const std::string& name)
		: _name{ name },
		  _readyForNextFrame{ true },
		  _exitFlag{ false },
		  _executionStage   { MCFRAME_RENDERER_EXECUTION_STAGE_IDLE } {

		// An executer should only be created on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		_pThread = std::make_unique<std::thread>(&MCFrameRendererExecuter::RenderLoop, this);
	}

	MCFrameRendererExecuter::~MCFrameRendererExecuter() {
		// An executer should only be destroyed on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		KillRenderer();
	}

	MC_RESULT MCFrameRendererExecuter::QueueNextFrame(MCFrame *pFrame, const MCFrameRendererTargetInfo& targetInfo) {

		if (!_readyForNextFrame.load())
			return MC_RESULT_FAIL_NOT_READY;

		assert(!_pNextFrame);

		_readyForNextFrame.store(false);

		_pNextFrame.reset(pFrame);
		_nextTargetInfo = targetInfo;

		return MC_RESULT_OK;
	}

	void MCFrameRendererExecuter::NotifyFramePresented() {
		assert(_executionStage.load() == MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT);
		_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_IDLE);
	}

	void MCFrameRendererExecuter::KillRenderer() {
		_exitFlag = true;
		if(_pThread && _pThread->joinable())
			_pThread->join();
	}

	void MCFrameRendererExecuter::RenderLoop() {

#ifdef __MC_THREAD_EXCEPTION_REPORTING__
		try {
#endif __MC_THREAD_EXCEPTION_REPORTING__

		MCThreads::RegisterCurrentThread(
			std::string(_name),
			MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER
		);

		while (!_exitFlag) {
			if (!_readyForNextFrame.load() && _executionStage.load() == MCFRAME_RENDERER_EXECUTION_STAGE_IDLE) {
				std::unique_ptr<MCFrame> _pCurrentFrame = std::move(_pNextFrame);
				_readyForNextFrame.store(true);
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_CPU_RENDERING);
				auto fenceValue = _pRenderer->RenderFrame(_pCurrentFrame.release(), _nextTargetInfo);
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_GPU_RENDERING);
				MCREGlobals::pMCD3D->WaitForFenceValue(fenceValue);
				_executionStage.store(MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT);
			}
		}

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
}
