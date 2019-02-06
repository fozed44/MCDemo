#define __ENABLE_INIT_TRACE__

#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../Render/MCD3DRenderEngine/src/Headers/DXGIWrapper.h"
#include "../../../Render/MCD3DRenderEngine/src/Headers/D3DWrapper.h"

#include <iostream>

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "dxgi.lib")

int main(int argc, char ** argv) {

	MC::MCLogHelper::SetDefaultLoggers();

	MC::MCLog::GetInstance().GetCore()->error("test int: {0:d}", 42);

	MC_ERROR("test float: {0:03.2f}", 42.23);
	MC_WARN("warning test {0:d}", 1);

	INIT_INFO("Test initialization. {0}", "test 1,2,3");

	auto test = MC::Load_MCXML_Document("C:\\Projects\\MCDemo\\bin\\Config\\core.config");

	auto r = test->Get("/note/to");

	int x;

	auto elem = r->ElementAt(0);

	elem->Attribute("testInt", &x);

	MC::RENDER_CONFIG renderConfig;
	MC::LoadRenderConfig(&renderConfig);

	auto dxgiWrapper = std::make_shared<MC::DXGIWrapper>();

	dxgiWrapper->Init(&renderConfig);

	auto d3dWrapper = std::make_shared<MC::D3DWrapper>();

	d3dWrapper->Init(&renderConfig, dxgiWrapper);

	char t;
	std::cin >> t;

	return 0;
}