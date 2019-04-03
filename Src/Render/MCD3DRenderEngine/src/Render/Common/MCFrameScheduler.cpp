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

#pragma endregion

	MC_RESULT MCFrameScheduler::QueueFrame(void *pFrame) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		unsigned int readyExecuterIndex;
		if (readyExecuterIndex = GetReadyExecuterIndex())
			return MC_RESULT_FAIL_NOT_READY;

		IncrementCounters();

		MCFrameRendererTargetInfo targetInfo;
		GetRenderTargetInfo(&targetInfo);

		auto queryFrameResult = _ppExecuters[readyExecuterIndex]->QueueNextFrame(pFrame, targetInfo);
		_presentQueue.push(readyExecuterIndex);
		assert(queryFrameResult == MC_RESULT_OK);

		return MC_RESULT_OK;
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

	MC_RESULT MCFrameScheduler::PresentNext() {
		if (_presentQueue.empty())
			return MC_RESULT_FAIL_NOT_READY;

		unsigned int nextExecuterIndex = _presentQueue.front();

		if (_ppExecuters[nextExecuterIndex]->QueryExecutionStage() == MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT) {
			MCREGlobals::pMCDXGI->Present();
			_ppExecuters[nextExecuterIndex]->NotifyFramePresented();
			ENTER_CRITICAL_SECTION(MCFrameScheduler_PresentNext, &_lock);
			_presentQueue.pop();
			EXIT_CRITICAL_SECTION;
			return MC_RESULT_OK;
		}

		return MC_RESULT_FAIL_NOT_READY;
	}
}