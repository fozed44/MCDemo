#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"

namespace MC {

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		RenderManager(RenderManager&) = delete;
		RenderManager(RenderManager&&) = delete;
		RenderManager& operator=(RenderManager&) = delete;
		RenderManager& operator=(RenderManager&&) = delete;
	};

}

