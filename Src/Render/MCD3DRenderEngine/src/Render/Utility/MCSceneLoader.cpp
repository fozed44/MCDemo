#include "MCSceneLoader.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"


namespace MC {

#pragma region ctor

	MCSceneLoader::MCSceneLoader(MCRenderScene* pScene) 
		: _pScene{ pScene } {
		// A scene loader should always be created on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
	}

	MCSceneLoader::~MCSceneLoader() {}

#pragma endregion

#pragma region Load Geometry

	

#pragma endregion

}