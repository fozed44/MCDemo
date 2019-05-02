#include "MCFrameScheduler.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../Core/MCMath.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"

namespace MC {

#pragma region ctor

	MCFrameScheduler::MCFrameScheduler() 
		: _nextExecuterIndex{ 0 } {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		for(unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x] = std::make_unique<MCFrameRendererExecuter>(std::string("Frame executer (") + std::to_string(x) + std::string(")"));
	}


	MCFrameScheduler::~MCFrameScheduler() {
	}

#pragma region Public

	MC_RESULT MCFrameScheduler::ScheduleFrame(MCFrame *pFrame) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		if (_frameQueue.size() >= FRAME_QUEUE_SIZE)
			return MC_RESULT::FAIL_NOT_READY;

		_frameQueue.push(std::unique_ptr<MCFrame>(pFrame));

		return MC_RESULT::OK;
	}

	void MCFrameScheduler::UpdateScheduler() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		TryQueueNextFrame();
	}

	void MCFrameScheduler::SetRenderers(MCFrameRenderer **ppRenderers, unsigned int count) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		if (nullptr == ppRenderers) {
			SafeReleaseCurrentRenderers();
		}
		else {
			for (unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x) {
				// If this assertion fires, make sure that the executer has had its renderers released.
				// This can be accomplished by calling this method with pRenderer == nullptr.
				assert(_ppExecuters[x]->QueryExecutionStage() == MCFRAME_RENDERER_EXECUTION_STAGE::NO_THREAD);
				_ppExecuters[x]->SetFrameRenderer(std::unique_ptr<MCFrameRenderer>(ppRenderers[x]));
			}
		}
	}

	MC_RESULT MCFrameScheduler::SuspendSync() {
		for (auto& executer : _ppExecuters) {
			auto result = executer->SuspendSync();

			if (result != MC_RESULT::OK)
				return result;
		}

		return MC_RESULT::OK;
	}

	void MCFrameScheduler::Unsuspend() {
		for (auto& executer : _ppExecuters)
			executer->Unsuspend();
	}

	void MCFrameScheduler::ReAcuireRenderTargets() {
		for (auto& executer : _ppExecuters)
			executer->ReAcquireRenderTarget();
	}

	void MCFrameScheduler::OnResizing() {
		for (auto& executer : _ppExecuters)
			executer->OnResizing();

		/* Resizing causes the MCD3D to make a call to ID3D12SwapChain->ResizeBuffers. This call resets the current back-buffer,
		   and in in turn, the next result of MCDXGI->GetCurrentBackBufferIndex will be 0. Since DX12 thinks the next buffer that
		   will need to be rendered is back-buffer 0, AND since _ppExecuters[0] will always render to back buffer 0, we need to
		   make sure that when the scheduler is un-suspended (we must be suspended if we are currently resizing) is starts back
		   up on the correct back buffer index.
		*/
		_nextExecuterIndex = 0;
	}

#pragma endregion

#pragma endregion Queue and Present

	void MCFrameScheduler::TryQueueNextFrame() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		
		if (_ppExecuters[_nextExecuterIndex]->QueryExecutionStage() != MCFRAME_RENDERER_EXECUTION_STAGE::IDLE
	     || _frameQueue.empty())
			return;
		
		auto nextFramePtr = std::move(_frameQueue.front());
		_frameQueue.pop();

		QueueFrame(nextFramePtr.release());
	}

	void MCFrameScheduler::QueueFrame(MCFrame *pFrame) {
		// This method is not thread safe. It can only be called from the main
		// thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		auto queryFrameResult = _ppExecuters[_nextExecuterIndex]->QueueNextFrame(pFrame);

		assert(queryFrameResult == MC_RESULT::OK);

		IncrementNextExecuterIndex();
	}

	void MCFrameScheduler::IncrementNextExecuterIndex() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		_nextExecuterIndex = ++_nextExecuterIndex % FRAME_BUFFER_COUNT;
	}
	
	void MCFrameScheduler::SafeReleaseCurrentRenderers() {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		for(unsigned int x = 0; x < FRAME_BUFFER_COUNT; ++x)
			_ppExecuters[x]->DestroyCurrentRenderer();
	}
}