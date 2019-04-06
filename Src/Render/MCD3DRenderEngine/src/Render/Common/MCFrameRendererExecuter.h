#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "MCFrameRenderer.h"

#include <atomic>
#include <thread>

namespace MC {

	typedef enum MCFRAME_RENDERER_EXECUTION_STAGE {
		MCFRAME_RENDERER_EXECUTION_STAGE_NO_THREAD,
		MCFRAME_RENDERER_EXECUTION_STAGE_IDLE,
		MCFRAME_RENDERER_EXECUTION_STAGE_CPU_RENDERING,
		MCFRAME_RENDERER_EXECUTION_STAGE_GPU_RENDERING,
		MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT
	} MCFRAME_RENDERER_EXECUTION_STAGE;

	class MCFrameRendererExecuter {
	public: /* ctor */
		MCFrameRendererExecuter(const std::string& name);
		~MCFrameRendererExecuter();

		MCFrameRendererExecuter(MCFrameRendererExecuter&)			  = delete;
		MCFrameRendererExecuter(MCFrameRendererExecuter&&)			  = delete;
		MCFrameRendererExecuter& operator=(MCFrameRendererExecuter&)  = delete;
		MCFrameRendererExecuter& operator=(MCFrameRendererExecuter&&) = delete;

	public: /* Render control */
		/*	Attempt to queue pNextFrame. Note that the scheduler *should* be using QueryReadyForNextFrame to
		ensure that a call to QueueNextFrame does not fail. Also if QueueNextFrame does not fail (and it should not)
		ownership of pNextFrame will be passed to the renderer, which will destroy the memory. */
		MC_RESULT QueueNextFrame(MCFrame *pNextFrame, const MCFrameRendererTargetInfo& targetInfo);

		/*	Notify the executer that its current frame has been presented to the screen. This will allow the
		executer from change states from WAITING_TO_PRESENT to IDLE. */
		void NotifyFramePresented();

		/*	Destroy the current renderer. The current render thread will be stopped. This method must be called before
		setting a new renderer. */
		void DestroyCurrentRenderer();

		/*	pRenderer will become the new renderer, the render thread will be started. It must be the case that 
		no renderer is currently set. DestroyCurrentRenderer must be called first. */
		void SetFrameRenderer(std::unique_ptr<MCFrameRenderer> pRenderer);

	public: /* query */

		MCFRAME_RENDERER_EXECUTION_STAGE QueryExecutionStage() const {
			return (MCFRAME_RENDERER_EXECUTION_STAGE)_executionStage.load();
		}

		bool QueryReadyForNextFrame() const { return _readyForNextFrame.load(); }

	private: /* Thread control */

		/* Start the render thread. _pRenderer must be valid and _pThread must be null. */
		void StartRenderThread();

		/* Stops the render thread. _pRenderer must be null. */
		void StopRenderThread();
		
	private: /* Render Loop */

		void RenderLoop();

	private:
		std::string                         _name;

		std::atomic<bool>					_readyForNextFrame;
		std::atomic<unsigned int>           _executionStage;
		bool                                _exitFlag;

		std::unique_ptr<std::thread>       _pThread;
		std::unique_ptr<MCFrameRenderer>   _pRenderer;

		std::unique_ptr<MCFrame>   	       _pNextFrame;
		MCFrameRendererTargetInfo          _nextTargetInfo;
	};

}

