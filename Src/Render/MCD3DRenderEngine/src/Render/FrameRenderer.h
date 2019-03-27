#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"

namespace MC {

	class FrameRenderer {
	public:
		FrameRenderer();
		~FrameRenderer();

		FrameRenderer(FrameRenderer&)			  = delete;
		FrameRenderer(FrameRenderer&&)			  = delete;
		FrameRenderer& operator=(FrameRenderer&)  = delete;
		FrameRenderer& operator=(FrameRenderer&&) = delete;

	public:
		void Render();

	};

}

