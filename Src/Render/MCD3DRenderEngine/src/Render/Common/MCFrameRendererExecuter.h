#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "MCFrameRenderer.h"

#include <atomic>
#include <thread>

namespace MC {

	typedef enum MCFRAME_RENDERER_EXECUTION_STAGE {
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
		MC_RESULT QueueNextFrame(MCFrame *pNextFrame, const MCFrameRendererTargetInfo& targetInfo);
		void NotifyFramePresented();

		void KillRenderer();

	public: /* query */

		MCFRAME_RENDERER_EXECUTION_STAGE QueryExecutionStage() const {
			return (MCFRAME_RENDERER_EXECUTION_STAGE)_executionStage.load();
		}

		bool QueryReadyForNextFrame() const { return _readyForNextFrame.load(); }

	private:
		std::string                         _name;

		std::atomic<bool>					_readyForNextFrame;
		std::atomic<unsigned int>           _executionStage;
		bool                                _exitFlag;

		void RenderLoop();
		std::unique_ptr<std::thread>       _pThread;
		std::unique_ptr<MCFrameRenderer>   _pRenderer;

		std::unique_ptr<MCFrame>   	       _pNextFrame;
		MCFrameRendererTargetInfo          _nextTargetInfo;
	};

}

