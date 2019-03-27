#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"

namespace MC {

	class RenderItem
	{
	public:
		RenderItem();
		~RenderItem();

		RenderItem(RenderItem&)             = delete;
		RenderItem(RenderItem&&)            = delete;
		RenderItem& operator=(RenderItem&)  = delete;
		RenderItem& operator=(RenderItem&&) = delete;

	public:
		void render();

	private:
		std::vector<std::unique_ptr<MCMesh>> _meshes;
	};

}

