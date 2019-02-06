#pragma once

#define RENDER_CONFIG_FILENAME "render.config"

namespace MC {

	struct RENDER_CONFIG {
		int   ADAPTOR_ORDINAL;
		int   OUTPUT_WIDTH;
		int   OUTPUT_HEIGHT;
		int   OUTPUT_REFRESH_RATE_NUMERATOR;
		int   OUTPUT_REFRESH_RATE_DENOMINATOR;


		inline void SetDefaults(RENDER_CONFIG &renderConfig) {
			renderConfig.ADAPTOR_ORDINAL     = 0;
			renderConfig.OUTPUT_WIDTH        = 800;
			renderConfig.OUTPUT_HEIGHT       = 600;
			renderConfig.OUTPUT_REFRESH_RATE_NUMERATOR = 500;
			renderConfig.OUTPUT_REFRESH_RATE_DENOMINATOR = 1000;
		}

	};

	void LoadRenderConfig(RENDER_CONFIG *pConfig);

}