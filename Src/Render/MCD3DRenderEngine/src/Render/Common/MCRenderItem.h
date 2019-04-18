#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"

namespace MC {

	class MCFrameRenderer;

	class MCRenderItem
	{
	public: /* ctor */
		MCRenderItem(MCFrameRenderer* pRenderer);
		~MCRenderItem();

		MCRenderItem(MCRenderItem&)             = delete;
		MCRenderItem(MCRenderItem&&)            = delete;
		MCRenderItem& operator=(MCRenderItem&)  = delete;
		MCRenderItem& operator=(MCRenderItem&&) = delete;

	public: /* API */
		MC_RESULT Render();

	private:
		MCFrameRenderer*                      _pRenderer;
		std::vector<std::unique_ptr<MCIMesh>> _meshes;
	};

}

