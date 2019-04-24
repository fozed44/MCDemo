#pragma once

#include "../Common/MCFrameRenderer.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRenderer3D : public MCFrameRenderer 
	{
	public: /* ctor / dtor / assignment */
		MCFrameRenderer3D(const std::string& name, unsigned int frameIndex, MCRenderScene* pScene);
		virtual ~MCFrameRenderer3D();
		MCFrameRenderer3D(MCFrameRenderer3D&)              = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)             = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&&) = delete;

	private: /* Initialize methods */
		void InitializeRenderer3D();
	};

}

