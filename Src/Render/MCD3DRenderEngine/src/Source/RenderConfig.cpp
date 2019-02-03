#include "../Headers/RenderConfig.h"
#include "../../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"

namespace MC {

	void LoadRenderConfig(RENDER_CONFIG *pRenderConfig) {

		auto renderConfigFilename = std::string(Paths::ConfigDirectory()) + RENDER_CONFIG_FILENAME;

		MC_INFO("**Loading render configuration.");

		auto pRenderConfigDoc = Load_MCXML_Document(renderConfigFilename.c_str());

		auto pRenderElementNodeSet = pRenderConfigDoc->Get("render");
		auto pRenderElement = pRenderElementNodeSet->ElementAt(0);

		pRenderElement->Attribute("adapterOrdinal", &pRenderConfig->ADAPTOR_ORDINAL);
		pRenderElement->Attribute("outputWidth",    &pRenderConfig->OUTPUT_WIDTH);
		pRenderElement->Attribute("outputHeight",   &pRenderConfig->OUTPUT_HEIGHT);
		pRenderElement->Attribute("refreshRate",    &pRenderConfig->OUTPUT_REFRESH_RATE);

		MC_INFO("**Loaded render configuration.");

	}

}