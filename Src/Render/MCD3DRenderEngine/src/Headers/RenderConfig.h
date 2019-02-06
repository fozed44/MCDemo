#pragma once

// This filename is used by LoadRenderConfig as the name of the render configuration. The location
// is determined by MCPaths.ConfigDirectory()
#define RENDER_CONFIG_FILENAME "render.config"

namespace MC {

	struct RENDER_CONFIG {
		int   ADAPTER_DEVICE_ID;
		int   OUTPUT_WIDTH;
		int   OUTPUT_HEIGHT;
		int   OUTPUT_REFRESH_RATE_NUMERATOR;
		int   OUTPUT_REFRESH_RATE_DENOMINATOR;


		inline void SetDefaults(RENDER_CONFIG &renderConfig) {
			renderConfig.ADAPTER_DEVICE_ID               = 0;
			renderConfig.OUTPUT_WIDTH                    = 800;
			renderConfig.OUTPUT_HEIGHT                   = 600;
			renderConfig.OUTPUT_REFRESH_RATE_NUMERATOR   = 500;
			renderConfig.OUTPUT_REFRESH_RATE_DENOMINATOR = 1000;
		}

	};

	void LoadRenderConfig(RENDER_CONFIG *pConfig);

}