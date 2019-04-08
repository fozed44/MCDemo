#include "MCFrameScheduler.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../Core/MCMath.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"

namespace MC {

#pragma region ctor

	MCFrameScheduler::MCFrameScheduler() 
		: _nextRenderTargetIndex{ 0 },
		  _nextDepthBufferIndex{ 0 } {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		for (unsigned int x = 0; x < EXECUTER_COUNT; ++x)
			_ppExecuters[x] = std::make_unique<MCFrameRendererExecuter>(std::string("Frame Executer (" + std::to_string(x) + std::string(")")));
		MCCriticalSection::InitializeLock(&_lock);
	}


	MCFrameScheduler::~MCFrameScheduler() {
	}

#pragma region Public

	MC_RESULT MCFrameScheduler::ScheduleFrame(MCFrame *pFrame) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
			return MC_RESULT_FAIL_NOT_READY;

		ENTER_CRITICAL_SECTION(MCFrameScheduler_PresentNext, &_lock);
			if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
				return MC_RESULT_FAIL_NOT_READY;
			_frameQueue.push(std::unique_ptr<MCFrame>(pFrame));
		EXIT_CRITICAL_SECTION

		return MC_RESULT_OK;
	}

	void MCFrameScheduler::UpdateSchedule() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("Enter UpdateSchedule----");
		TryPresentNext();
		TryQueueNextFrame();
	}

	void MCFrameScheduler::SetRenderers(MCFrameRenderer **ppRenderers, unsigned int numRenderers) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		assert(numRenderers == 0 || numRenderers == EXECUTER_COUNT);

		if (nullptr == ppRenderers) {
			SafeReleaseCurrentRenderers();
		}
		else {
			for (unsigned int x = 0; x < EXECUTER_COUNT; ++x) {
				// If this assertion fires, make sure that the executers have had their renderers released.
				// This can be accomplished by calling this method with ppRenderers == nullptr.
				assert(_ppExecuters[x]->QueryExecutionStage() == MCFRAME_RENDERER_EXECUTION_STAGE_NO_THREAD);
				_ppExecuters[x]->SetFrameRenderer(std::unique_ptr<MCFrameRenderer>(ppRenderers[x]));
			}
		}
	}

#pragma endregion

#pragma endregion Queue and Present

	void MCFrameScheduler::TryQueueNextFrame() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("--Enter TryQueueNextFrame");
		if (_frameQueue.empty()) {
			RENDER_TRACE("----TryQueueNextFrame: _frameQueue.empty.");
			return;
		}

		int readyExecuterIndex;
		if ((readyExecuterIndex = GetReadyExecuterIndex()) == -1) {
			RENDER_TRACE("----TryQueueNextFrame: readyExecuterIndex = -1");
			return;
		}

		RENDER_TRACE("----TryQueueNextFrame: readyExecuterIndex: {0:d}", readyExecuterIndex);

		std::unique_ptr<MCFrame> nextFramePtr;

		ENTER_CRITICAL_SECTION(MCFrameScheduler_UpdateSchedule, &_lock);
			if (_frameQueue.empty())
				return;
			nextFramePtr = std::move(_frameQueue.front());
			_frameQueue.pop();
		EXIT_CRITICAL_SECTION

		QueueFrame(nextFramePtr.release(), readyExecuterIndex);
	}

	void MCFrameScheduler::QueueFrame(MCFrame *pFrame, unsigned int executerIndex) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("----Enter QueueFrame: _presentQueue.size: {0:d}", _presentQueue.size());


		MCFrameRendererTargetInfo targetInfo;
		GetRenderTargetInfo(&targetInfo);

		auto queryFrameResult = _ppExecuters[executerIndex]->QueueNextFrame(pFrame, targetInfo);

		IncrementCounters();
		_presentQueue.push(executerIndex);
		assert(queryFrameResult == MC_RESULT_OK);
	}

	void MCFrameScheduler::IncrementCounters() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		_nextRenderTargetIndex = ++_nextRenderTargetIndex % FRAME_BUFFER_COUNT;
		_nextDepthBufferIndex  = ++_nextDepthBufferIndex  % DEPTH_BUFFER_COUNT;
	}

	int MCFrameScheduler::GetReadyExecuterIndex() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("----Enter GetReadyExecuterIndex.");
		for (int x = 0; x < EXECUTER_COUNT; ++x)
			if (_ppExecuters[x]->QueryReadyForNextFrame()) {
				RENDER_INFO("------GetReadyExecuterIndex: x = {0:d}", x);
				return x;
			}

		RENDER_INFO("------GetReadyExecuterIndex: x = -1");
		return -1;
	}

	void MCFrameScheduler::GetRenderTargetInfo(MCFrameRendererTargetInfo *pInfo) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("------Enter GetRenderTargetInfo");
		static int lastBufferIndex = -1; static int lastRenderTargetIndex = 0;
		auto currentBackBuffer = MCREGlobals::pMCDXGI->GetCurrentBackBufferIndex();
		auto firstResult = currentBackBuffer;
		unsigned int x;
		for (x = 0; x < 10000000; x++) {
			currentBackBuffer = MCREGlobals::pMCDXGI->GetCurrentBackBufferIndex();
			//if (currentBackBuffer != MCMath::Abs(((int)_nextRenderTargetIndex - 1) % FRAME_BUFFER_COUNT))
			if(((FRAME_BUFFER_COUNT + currentBackBuffer - 1) % FRAME_BUFFER_COUNT) != _nextRenderTargetIndex)
				break;

		}
		if (currentBackBuffer != firstResult) {
			RENDER_INFO("--------Prevented error first result {0}, second result {1}, x {2}", firstResult, currentBackBuffer, x);
		}
		RENDER_INFO("--------lastBufferIndex = {0:d}, currentBackBuffer = {1:d}, lastRenderTargetIndex = {2:d}, nextRenderTargetIndex = {3:d}", lastBufferIndex, currentBackBuffer, lastRenderTargetIndex, _nextRenderTargetIndex);
		//assert(((FRAME_BUFFER_COUNT + currentBackBuffer - 1) % FRAME_BUFFER_COUNT)!= _nextRenderTargetIndex);
		//assert((lastBufferIndex + 1) % FRAME_BUFFER_COUNT == currentBackBuffer);
		lastBufferIndex = currentBackBuffer; lastRenderTargetIndex = _nextRenderTargetIndex;

		pInfo->FrameIndex = _nextRenderTargetIndex;
		pInfo->pRenderTarget    = MCREGlobals::pMCD3D->GetRenderTarget         (_nextRenderTargetIndex);
		pInfo->hCPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetCPUHandle(_nextRenderTargetIndex);
		pInfo->hGPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetGPUHandle(_nextRenderTargetIndex);

		pInfo->pDepthStencil    = MCREGlobals::pMCD3D->GetDepthStencil         (_nextDepthBufferIndex);
		pInfo->hCPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilCPUHandle(_nextDepthBufferIndex);
		pInfo->hGPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilGPUHandle(_nextDepthBufferIndex);		
	}

	void MCFrameScheduler::TryPresentNext() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		RENDER_TRACE("--Enter TryPresentNext");
		if (_presentQueue.empty()) {
			RENDER_TRACE("----TryPresentNext: empty.");
			return;
		}

		unsigned int nextExecuterIndex = _presentQueue.front();
		auto executionStage = _ppExecuters[nextExecuterIndex]->QueryExecutionStage();
		if (executionStage == MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT) {
			auto preBackBuffer = MCREGlobals::pMCDXGI->GetCurrentBackBufferIndex();
			RENDER_TRACE("----TryPresentNext: Presenting when currentBackBuffer: {0:d}, and nextExecuterIndex {1:0}", preBackBuffer, nextExecuterIndex);
			MCREGlobals::pMCDXGI->Present();
			MCREGlobals::pMCD3D->FlushCommandQueue();
			_ppExecuters[nextExecuterIndex]->NotifyFramePresented();
			
			_presentQueue.pop();
			auto postBackBuffer = MCREGlobals::pMCDXGI->GetCurrentBackBufferIndex();
			//assert(postBackBuffer == (preBackBuffer + 1) % FRAME_BUFFER_COUNT);
		}
		else {
			RENDER_TRACE("----TryPresentNext: Executer not ready, stage: {0:d}", executionStage);
		}
	}
	
	void MCFrameScheduler::SafeReleaseCurrentRenderers() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		for (unsigned int x = 0; x < EXECUTER_COUNT; ++x)
			_ppExecuters[x]->DestroyCurrentRenderer();
	}
}