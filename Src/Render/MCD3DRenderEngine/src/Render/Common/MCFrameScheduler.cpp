#include "MCFrameScheduler.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../Core/MCMath.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"

namespace MC {

#pragma region ctor

	MCFrameScheduler::MCFrameScheduler() 
		: _nextExecuterIndex{ 0 } {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		for(unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x] = std::make_unique<MCFrameRendererExecuter>(std::string("Frame executer (") + std::to_string(x) + std::string(")"));
		MCCriticalSection::InitializeLock(&_lock);
	}


	MCFrameScheduler::~MCFrameScheduler() {
	}

#pragma region Public

	MC_RESULT MCFrameScheduler::ScheduleFrame(MCFrame *pFrame) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
			return MC_RESULT_FAIL_NOT_READY;

		if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
			return MC_RESULT_FAIL_NOT_READY;
		_frameQueue.push(std::unique_ptr<MCFrame>(pFrame));

		return MC_RESULT_OK;
	}

	void MCFrameScheduler::UpdateScheduler() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		//TryPresent();
		TryQueueNextFrame();
	}

	void MCFrameScheduler::SetRenderers(MCFrameRenderer **ppRenderers, unsigned int count) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		if (nullptr == ppRenderers) {
			SafeReleaseCurrentRenderers();
		}
		else {
			for (unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x) {
				// If this assertion fires, make sure that the executer has had its renderers released.
				// This can be accomplished by calling this method with pRenderer == nullptr.
				assert(_ppExecuters[x]->QueryExecutionStage() == MCFRAME_RENDERER_EXECUTION_STAGE_NO_THREAD);
				_ppExecuters[x]->SetFrameRenderer(std::unique_ptr<MCFrameRenderer>(ppRenderers[x]));
			}
		}
	}

#pragma endregion

#pragma endregion Queue and Present

	void MCFrameScheduler::TryQueueNextFrame() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		
		if (_ppExecuters[_nextExecuterIndex]->QueryExecutionStage() != MCFRAME_RENDERER_EXECUTION_STAGE_IDLE
	     || _frameQueue.empty())
			return;
		
		auto nextFramePtr = std::move(_frameQueue.front());
		_frameQueue.pop();

		QueueFrame(nextFramePtr.release());
	}

	void MCFrameScheduler::QueueFrame(MCFrame *pFrame) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		/*MCFrameRendererTargetInfo targetInfo;
		GetRenderTargetInfo(&targetInfo);*/

		auto queryFrameResult = _ppExecuters[_nextExecuterIndex]->QueueNextFrame(pFrame);
		assert(queryFrameResult == MC_RESULT_OK);

		IncrementRenderTargetIndex();
	}

	void MCFrameScheduler::IncrementRenderTargetIndex() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		_nextExecuterIndex = ++_nextExecuterIndex % FRAME_BUFFER_COUNT;
	}

	/*void MCFrameScheduler::GetRenderTargetInfo(MCFrameRendererTargetInfo *pInfo) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		pInfo->FrameIndex       = _nextRenderTargetIndex;
		pInfo->pRenderTarget    = MCREGlobals::pMCD3D->GetRenderTarget         (_nextRenderTargetIndex);
		pInfo->hCPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetCPUHandle(_nextRenderTargetIndex);
		pInfo->hGPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetGPUHandle(_nextRenderTargetIndex);
	}*/

	/*void MCFrameScheduler::TryPresent() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		if (_pExecuter->QueryExecutionStage() != MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT)
			return;

		MCREGlobals::pMCDXGI->Present();
		_pExecuter->NotifyFramePresented();		
	}*/
	
	void MCFrameScheduler::SafeReleaseCurrentRenderers() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		for(unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x]->DestroyCurrentRenderer();
	}
}