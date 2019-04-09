#include "MCFrameRenderer3D.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../Core/MCREGlobals.h"

namespace MC {

#pragma region ctor

	MCFrameRenderer3D::MCFrameRenderer3D(const std::string& name, unsigned int frameIndex) 
		: MCFrameRenderer(name, frameIndex) {
		InitializeRenderer3D();
	}


	MCFrameRenderer3D::~MCFrameRenderer3D() {
		// A frame renderer should only be DESTROYED on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);
	}	

#pragma endregion

#pragma region Initialization

	void MCFrameRenderer3D::InitializeRenderer3D() {
		INIT_TRACE("Begin initialize renderer3d {0}.", Name());
		INIT_TRACE("End initialize renderer3D {0}.", Name());
	}	

#pragma endregion



}