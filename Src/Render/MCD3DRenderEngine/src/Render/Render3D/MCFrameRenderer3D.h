#pragma once

#include "../Common/MCFrameRendererBase.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRenderer3D : public MCFrameRendererBase 
	{
	public: /* ctor / dtor / assignment */
		MCFrameRenderer3D(const std::string& name, unsigned int frameIndex);
		~MCFrameRenderer3D();
		MCFrameRenderer3D(MCFrameRenderer3D&)              = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)             = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&&) = delete;

	private: /* Initialize methods */
		void InitializeRenderer3D();

	};

}

