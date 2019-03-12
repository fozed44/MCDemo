#include "../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../Render/MCD3DRenderEngine/src/Headers/DXGIWrapper.h"
#include "../../../Render/MCD3DRenderEngine/src/Headers/D3DWrapper.h"
#include "../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../Render/MCD3DRenderEngine//src/Headers/MCRenderWindow.h"
#include "../../../Common/MCCommon/src/Headers/MasterTimer.h"
#include "../../../Common/MCCommon/src/Headers/MCFrame.h"

#include <iostream>

#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "dxgi.lib")

int Sandbox() {
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

	auto masterTimer = std::make_unique<MC::MasterTimer>();
	masterTimer->Reset();

	MC::RENDER_CONFIG renderConfig;
	MC::LoadRenderConfig(&renderConfig);

	auto pRenderWindow = std::make_shared<MC::MCRenderWindow>(renderConfig);

	pRenderWindow->Init();

	auto dxgiWrapper = std::make_shared<MC::DXGIWrapper>();

	dxgiWrapper->Init(&renderConfig, pRenderWindow);

	auto d3dWrapper = std::make_unique<MC::D3DWrapper>(renderConfig);

	d3dWrapper->Init(dxgiWrapper);

	pRenderWindow->RegisterResizeCallback(
		[dxgiWrapper]() {
			dxgiWrapper->QueueResize();
		}
	);

	MSG msg = {};
	
	float frameCount = 0;

	MC::MCFrame frame;
	frame.CameraPosition = { 0.0f, 0.0f, -10.0f, 1.0f };
	frame.LookAt         = { 0.0f, 0.0f, 0.0f, 0.0f };
	

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			masterTimer->Tick();

			frame.CameraPosition.x = pRenderWindow->GetRadius()*sinf(pRenderWindow->GetPhi())*cosf(pRenderWindow->GetTheta());
			frame.CameraPosition.z = pRenderWindow->GetRadius()*sinf(pRenderWindow->GetPhi())*sinf(pRenderWindow->GetTheta());
			frame.CameraPosition.y = pRenderWindow->GetRadius()*cosf(pRenderWindow->GetPhi());
			frame.Time = masterTimer->TotalTime();

			d3dWrapper->RenderFrame(&frame);

			frameCount++;

			

			float fps = frameCount / masterTimer->TotalTime();

			std::string t = std::string("MCDemo Frame(") + std::to_string(frameCount) + std::string(") fps: ") + std::to_string(fps);

			SetWindowTextA(pRenderWindow->hWnd(), t.c_str());

		}
	}
	
	return 0;
}

int main(int argc, char ** argv) {
	try {
		return Sandbox();
	}
	catch (MCException *ex) {
		MessageBox(nullptr, ex->what().c_str(), "MCException", MB_OK);
	}
	catch (MCException ex) {
		MessageBox(nullptr, ex.what().c_str(), "MCException", MB_OK);
	}
}
