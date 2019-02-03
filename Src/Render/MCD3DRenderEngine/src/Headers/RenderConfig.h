#pragma once

#define RENDER_CONFIG_FILENAME "render.config"

namespace MC {

	struct RENDER_CONFIG {
		int   ADAPTOR_ORDINAL;
		int   OUTPUT_WIDTH;
		int   OUTPUT_HEIGHT;
		float OUTPUT_REFRESH_RATE;
	};

	void LoadRenderConfig(RENDER_CONFIG *pConfig);

}