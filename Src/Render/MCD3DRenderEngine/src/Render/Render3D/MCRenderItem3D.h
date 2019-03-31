#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"

namespace MC {

	class MCRenderItem3D
	{
	public:
		MCRenderItem3D();
		~MCRenderItem3D();

		MCRenderItem3D(MCRenderItem3D&)             = delete;
		MCRenderItem3D(MCRenderItem3D&&)            = delete;
		MCRenderItem3D& operator=(MCRenderItem3D&)  = delete;
		MCRenderItem3D& operator=(MCRenderItem3D&&) = delete;

	public:
		void render();

	private:
		std::vector<std::unique_ptr<MCMesh>> _meshes;
	};

}

