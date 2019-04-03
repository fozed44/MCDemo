#include "MCFrameScheduler.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

#pragma region ctor

	MCFrameScheduler::MCFrameScheduler() 
		: _nextRenderTargetIndex{ 0 },
		  _nextDepthBufferIndex{ 0 } {
		for (unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x] = std::make_unique<MCFrameRendererExecuter>();
		MCCriticalSection::InitializeLock(&_lock);
	}


	MCFrameScheduler::~MCFrameScheduler() {
	}

#pragma region Public

	MC_RESULT MCFrameScheduler::ScheduleFrame(void *pFrame) {
		if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
			return MC_RESULT_FAIL_NOT_READY;

		ENTER_CRITICAL_SECTION(MCFrameScheduler_PresentNext, &_lock);
			if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
				return MC_RESULT_FAIL_NOT_READY;
			_frameQueue.push(std::make_unique<void>(pFrame));
		EXIT_CRITICAL_SECTION
	}

	void MCFrameScheduler::UpdateSchedule() {
		TryPresentNext();
		TryQueueNextFrame();
	}

#pragma endregion

#pragma endregion Queue and Present

	void MCFrameScheduler::TryQueueNextFrame() {
		if (_frameQueue.empty())
			return;

		int readyExecuterIndex;
		if ((readyExecuterIndex = GetReadyExecuterIndex()) == -1)
			return;

		std::unique_ptr<void> nextFramePtr;


		ENTER_CRITICAL_SECTION(MCFrameScheduler_UpdateSchedule, &_lock);
			if (_frameQueue.empty())
				return;
			nextFramePtr = std::move(_frameQueue.front);
			_frameQueue.pop();
		EXIT_CRITICAL_SECTION
	}

	void MCFrameScheduler::QueueFrame(void *pFrame, unsigned int executerIndex) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		IncrementCounters();

		MCFrameRendererTargetInfo targetInfo;
		GetRenderTargetInfo(&targetInfo);

		auto queryFrameResult = _ppExecuters[executerIndex]->QueueNextFrame(pFrame, targetInfo);
		_presentQueue.push(executerIndex);
		assert(queryFrameResult == MC_RESULT_OK);
	}

	void MCFrameScheduler::IncrementCounters() {
		_nextRenderTargetIndex = ++_nextRenderTargetIndex % FRAME_BUFFER_COUNT;
		_nextDepthBufferIndex  = ++_nextDepthBufferIndex  % DEPTH_BUFFER_COUNT;
	}

	int MCFrameScheduler::GetReadyExecuterIndex() {
		for (int x = 0; x < EXECUTER_COUNT; ++x)
			if (_ppExecuters[x]->QueryReadyForNextFrame())
				return x;
		return -1;
	}

	void MCFrameScheduler::GetRenderTargetInfo(MCFrameRendererTargetInfo *pInfo) {
		pInfo->pRenderTarget    = MCREGlobals::pMCD3D->GetRenderTarget         (_nextRenderTargetIndex);
		pInfo->hCPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetCPUHandle(_nextRenderTargetIndex);
		pInfo->hGPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetGPUHandle(_nextRenderTargetIndex);

		pInfo->pDepthStencil    = MCREGlobals::pMCD3D->GetDepthStencil         (_nextDepthBufferIndex);
		pInfo->hCPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilCPUHandle(_nextDepthBufferIndex);
		pInfo->hGPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilGPUHandle(_nextDepthBufferIndex);		
	}

	void MCFrameScheduler::TryPresentNext() {
		if (_presentQueue.empty())
			return;

		unsigned int nextExecuterIndex = _presentQueue.front();

		if (_ppExecuters[nextExecuterIndex]->QueryExecutionStage() == MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT) {
			MCREGlobals::pMCDXGI->Present();
			_ppExecuters[nextExecuterIndex]->NotifyFramePresented();
			
			_presentQueue.pop();
		}
	}
}