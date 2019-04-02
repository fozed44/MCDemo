#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "MCFrameRenderer3D.h"

#include <atomic>
#include <thread>

namespace MC {

	typedef enum MCFRAME_RENDERER_EXECUTION_STAGE {
		MCFRAME_RENDERER_EXECUTION_STAGE_IDLE,
		MCFRAME_RENDERER_EXECUTION_STAGE_CPU_RENDERING,
		MCFRAME_RENDERER_EXECUTION_STAGE_GPU_RENDERING,
		MCFRAME_RENDERER_EXECUTION_STAGE_WAITING_TO_PRESENT
	} MCFRAME_RENDERER_EXECUTION_STAGE;

	class MCFrameRendererExecuter3D {
	public:
		MCFrameRendererExecuter3D();
		~MCFrameRendererExecuter3D();

		MCFrameRendererExecuter3D(MCFrameRendererExecuter3D&)			  = delete;
		MCFrameRendererExecuter3D(MCFrameRendererExecuter3D&&)			  = delete;
		MCFrameRendererExecuter3D& operator=(MCFrameRendererExecuter3D&)  = delete;
		MCFrameRendererExecuter3D& operator=(MCFrameRendererExecuter3D&&) = delete;

	public:
		MC_RESULT QueueNextFrame(void *pNextFrame, const MCFrameRendererTargetInfo& targetInfo);
		void NotifyFramePresented();

	private:
		std::atomic<bool>					_readyForNextFrame;
		std::atomic<unsigned int>           _executionStage;

		void RenderLoop();
		std::unique_ptr<std::thread>       _pThread;
		std::unique_ptr<MCFrameRenderer3D> _pRenderer;

		std::unique_ptr<void>   	       _pNextFrame;
		MCFrameRendererTargetInfo          _nextTargetInfo;
	private:
		static std::atomic_uint s_nextThreadID;
	};

}

