#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D.h"


namespace MC {

	class MCFrameRenderer3D
	{
	public:
		MCFrameRenderer3D();
		~MCFrameRenderer3D();
		MCFrameRenderer3D(MCFrameRenderer3D&)              = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)             = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&&) = delete;

	public:
		void Render(const MCFrame3D& frame);
	};

}

