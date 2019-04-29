#include "MCRenderScene.h"

#include "../Utility/MCSceneLoader.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"

namespace MC {

#pragma region ctor

	MCRenderScene::MCRenderScene() { 
		// A Scene should only be created on the main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		// Every time a new render scene is created, a new scene loader
		// must be created for it.
		_pSceneLoader = std::make_unique<MCSceneLoader>(this);
	}

	MCRenderScene::~MCRenderScene() { }

#pragma endregion

	HRenderItem MCRenderScene::AddRenderItem(std::unique_ptr<MCRenderItem> pRenderItem) {
		// This function does not use any thread synchronization. It must be executed on the
		// main thread.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		_renderItems.push_back(std::move(pRenderItem));
		return pRenderItem.get();
	}

}