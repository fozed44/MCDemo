#include "MCFrameRenderer3D.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
namespace MC {

	MCFrameRenderer3D::MCFrameRenderer3D() {
		// A frame renderer should only be CREATED on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);
	}


	MCFrameRenderer3D::~MCFrameRenderer3D() {
		// A frame renderer should only be DESTROYED on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);
	}

	void MCFrameRenderer3D::Render(const MCFrame3D& frame) {
		// Render should only be called by a render executer, on an executer thread.
		MCThreads::Assert(MC_THREAD_CLASS_FRAME_RENDERER_EXECUTER);
	}

}