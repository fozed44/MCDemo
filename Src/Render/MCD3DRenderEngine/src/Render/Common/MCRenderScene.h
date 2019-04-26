#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../Render/Common/MCRenderItem.h"
#include <vector>

namespace MC {

	class MCSceneLoader;

	class MCRenderScene {
	public:
		using HRenderItem = MCRenderItem*;
	public:
		MCRenderScene();
		virtual ~MCRenderScene();
		MCRenderScene(MCRenderScene&)              = delete;
		MCRenderScene(MCRenderScene&&)             = delete;
		MCRenderScene& operator= (MCRenderScene&)  = delete;
		MCRenderScene& operator= (MCRenderScene&&) = delete;
	public: /* Add API */

		/*	When a new render item is added to the scene via AddRenderItem, the scene takes
			control of the memory backing the render item. 
			
			In general the scene loader (GetSceneLoader) should be used to populate render
			items. */
		HRenderItem AddRenderItem(std::unique_ptr<MCRenderItem> pRenderItem);

		/* Get the scene loader. The scene loader should be used to load items into
		   the scene. 
		   
			In general, the scene loader does pre processing that is dependent on the render item
			type, it creates the verticies, indicies, etc, but once the data is loaded it always
			will end up calling MCRenderScene::AddRenderItem to add the item to the scene. */
		const MCSceneLoader* GetSceneLoader() { return _pSceneLoader.get(); }

	private: 
		std::vector<std::unique_ptr<MCRenderItem>> _renderItems;
		std::unique_ptr<MCSceneLoader>             _pSceneLoader;
	};

	using HRenderItem = MCRenderScene::HRenderItem;

}