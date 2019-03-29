#include "MCFrameRendererExecuter3D.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"


namespace MC {

	std::atomic_uint s_nextThreadID{ 0 };

	MCFrameRendererExecuter3D::MCFrameRendererExecuter3D()
		: _readyForNextFrame{true} {
		_pThread = std::make_unique<std::thread>(&MCFrameRendererExecuter3D::RenderLoop, this);
	}

	MCFrameRendererExecuter3D::~MCFrameRendererExecuter3D() { }

	MC_RESULT MCFrameRendererExecuter3D::QueueNextFrame(MCFrame3D *pFrame) {

		if (!_readyForNextFrame.load())
			return MC_RESULT_FAIL_NOT_READY;

		_pNextFrame.reset(pFrame);

		assert(_readyForNextFrame.load()); // should ALWAYS be true;
		_readyForNextFrame.store(false);
	}

	void MCFrameRendererExecuter3D::RenderLoop() {

		MCThreads::RegisterCurrentThread(
			std::string("MCFrameRender3D Thread ") + std::to_string(s_nextThreadID++),
			MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER
		);

		while (1) {
			if (!_readyForNextFrame.load()) {
				_pCurrentFrame = std::move(_pNextFrame);
				assert(!_readyForNextFrame.load()); // should ALWAYS be false;
				_readyForNextFrame.store(true); 
			}

			if (_pCurrentFrame) {

			}
		}
	}
}
