#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"

namespace MC {

	void LoadRenderConfig(RENDER_CONFIG *pRenderConfig) {

		// TODO:
		//  Add error checking/handling for the reading of this XML file.

		auto renderConfigFilename = std::string(Paths::ConfigDirectory()) + RENDER_CONFIG_FILENAME;

		MC_INFO("**Loading renderer configuration.");

		auto pRenderConfigDoc = Load_MCXML_Document(renderConfigFilename.c_str());

		auto pDisplayElementNodeSet = pRenderConfigDoc->Get("render/display");
		auto pDisplayElement = pDisplayElementNodeSet->ElementAt(0);

		pDisplayElement->Attribute("adapterDeviceID", &pRenderConfig->DISPLAY_ADAPTER_DEVICE_ID);
		pDisplayElement->Attribute("outputWidth",     &pRenderConfig->DISPLAY_OUTPUT_WIDTH);
		pDisplayElement->Attribute("outputHeight",    &pRenderConfig->DISPLAY_OUTPUT_HEIGHT);
		pDisplayElement->Attribute("refreshRateN",    &pRenderConfig->DISPLAY_OUTPUT_REFRESH_RATE_NUMERATOR);
		pDisplayElement->Attribute("refreshRateD",    &pRenderConfig->DISPLAY_OUTPUT_REFRESH_RATE_DENOMINATOR);
		pDisplayElement->Attribute("fullscreen",      &pRenderConfig->DISPLAY_FULLSCREEN);

		auto pDebugElementNodeSet = pRenderConfigDoc->Get("render/debug");
		auto pDebugElement = pDebugElementNodeSet->ElementAt(0);
		
		pDebugElement->Attribute("enableDXDebug", &pRenderConfig->DEBUG_ENABLE_DX_DEBUG);
		
		auto pRenderElementNodeSet = pRenderConfigDoc->Get("render/options");
		auto pRenderElement = pRenderElementNodeSet->ElementAt(0);

		pRenderElement->Attribute("multisample", &pRenderConfig->OPTIONS_MULTISAMPLE);

		MC_INFO("**Loaded renderer configuration.");

	}

}