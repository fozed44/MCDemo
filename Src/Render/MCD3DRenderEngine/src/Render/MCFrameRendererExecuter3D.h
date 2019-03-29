#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"
#include "MCFrameRenderer3D.h"

#include <atomic>
#include <thread>

namespace MC {

	class MCFrameRendererExecuter3D {
	public:
		MCFrameRendererExecuter3D();
		~MCFrameRendererExecuter3D();

		MCFrameRendererExecuter3D(MCFrameRendererExecuter3D&)			  = delete;
		MCFrameRendererExecuter3D(MCFrameRendererExecuter3D&&)			  = delete;
		MCFrameRendererExecuter3D& operator=(MCFrameRendererExecuter3D&)  = delete;
		MCFrameRendererExecuter3D& operator=(MCFrameRendererExecuter3D&&) = delete;

	public:
		MC_RESULT QueueNextFrame(MCFrame3D *pNextFrame);

	private:
		std::atomic_bool _readyForNextFrame;
		std::atomic_bool _empty;

		void RenderLoop();
		std::unique_ptr<std::thread>       _pThread;
		std::unique_ptr<MCFrame3D>	       _pCurrentFrame;
		std::unique_ptr<MCFrame3D>	       _pNextFrame;
		std::unique_ptr<MCFrameRenderer3D> _pRenderer;
	private:
		static std::atomic_uint s_nextThreadID;
	};

}

