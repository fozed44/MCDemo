#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"

#include <atomic>
#include <thread>

namespace MC {

	class MCFrameRenderer3D {
	public:
		MCFrameRenderer3D();
		~MCFrameRenderer3D();

		MCFrameRenderer3D(MCFrameRenderer3D&)			  = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)			  = delete;
		MCFrameRenderer3D& operator=(MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator=(MCFrameRenderer3D&&) = delete;

	public:
		MC_RESULT QueueNextFrame(MCFrame3D *pNextFrame);

	private:
		std::atomic_bool _readyForNextFrame;
		std::atomic_bool _empty;

		void ThreadPump();
		std::unique_ptr<std::thread> _pThread;
		std::unique_ptr<MCFrame3D>	 _pCurrentFrame;
		std::unique_ptr<MCFrame3D>	 _pNextFrame;
	};

}

