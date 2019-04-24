#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../Render/Common/MCRenderItem.h"
#include <vector>

namespace MC {

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
			control of the memory backing the render item. */
		HRenderItem AddRenderItem(MCRenderItem* pRenderItem);

	public: /* Get API */
		std::vector<std::unique_ptr<MCRenderItem>> _renderItems;
	};

	using HRenderItem = MCRenderScene::HRenderItem;

}