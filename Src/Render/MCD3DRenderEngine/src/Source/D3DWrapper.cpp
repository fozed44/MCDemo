#pragma once

#include "../Headers/D3DWrapper.h"
#include "../Headers/RenderConfig.h"

namespace MC {

	D3DWrapper::D3DWrapper() {}

	D3DWrapper::~D3DWrapper() {}

	void D3DWrapper::Init() {

		RENDER_CONFIG renderConfig;

		LoadRenderConfig(&renderConfig);

	}
}
