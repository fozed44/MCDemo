#pragma once

// This filename is used by LoadRenderConfig as the name of the render configuration. The location
// is determined by MCPaths.ConfigDirectory()
#define RENDER_CONFIG_FILENAME "render.config"

namespace MC {

	struct RENDER_CONFIG {
		int   DEBUG_ENABLE_DX_DEBUG;
		int   DISPLAY_ADAPTER_DEVICE_ID;
		int   DISPLAY_OUTPUT_WIDTH;
		int   DISPLAY_OUTPUT_HEIGHT;
		int   DISPLAY_OUTPUT_REFRESH_RATE_NUMERATOR;
		int   DISPLAY_OUTPUT_REFRESH_RATE_DENOMINATOR;
		int   DISPLAY_FULLSCREEN;


		static inline void SetDefaults(RENDER_CONFIG *pRenderConfig) {
			pRenderConfig->DEBUG_ENABLE_DX_DEBUG                   = 0;
			pRenderConfig->DISPLAY_ADAPTER_DEVICE_ID               = 0;
			pRenderConfig->DISPLAY_OUTPUT_WIDTH                    = 800;
			pRenderConfig->DISPLAY_OUTPUT_HEIGHT                   = 600;
			pRenderConfig->DISPLAY_OUTPUT_REFRESH_RATE_NUMERATOR   = 500;
			pRenderConfig->DISPLAY_OUTPUT_REFRESH_RATE_DENOMINATOR = 1000;
			pRenderConfig->DISPLAY_FULLSCREEN                      = 0;
		}

	};

	void LoadRenderConfig(RENDER_CONFIG *pConfig);

}