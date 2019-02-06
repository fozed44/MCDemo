#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"

namespace MC {

	void LoadRenderConfig(RENDER_CONFIG *pRenderConfig) {

		auto renderConfigFilename = std::string(Paths::ConfigDirectory()) + RENDER_CONFIG_FILENAME;

		MC_INFO("**Loading renderer configuration.");

		auto pRenderConfigDoc = Load_MCXML_Document(renderConfigFilename.c_str());

		auto pRenderElementNodeSet = pRenderConfigDoc->Get("render/output");
		auto pRenderElement = pRenderElementNodeSet->ElementAt(0);

		pRenderElement->Attribute("adapterDeviceID", &pRenderConfig->ADAPTER_DEVICE_ID);
		pRenderElement->Attribute("outputWidth",     &pRenderConfig->OUTPUT_WIDTH);
		pRenderElement->Attribute("outputHeight",    &pRenderConfig->OUTPUT_HEIGHT);
		pRenderElement->Attribute("refreshRateN",    &pRenderConfig->OUTPUT_REFRESH_RATE_NUMERATOR);
		pRenderElement->Attribute("refreshRateD",    &pRenderConfig->OUTPUT_REFRESH_RATE_DENOMINATOR);

		MC_INFO("**Loaded renderer configuration.");

	}

}