#include "../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCDXGI.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCD3D.h"
#include "../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCRenderWindow.h"
#include "../../../Common/MCCommon/src/Headers/MasterTimer.h"
#include "../../../Common/MCCommon/src/Headers/MCFrame.h"
#include "../../../Common/MCCommon/src/Headers/MCCriticalSection.h"
#include <iostream>
#include <thread>
#include <atomic>
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

	MC::MCDXGI::Instance()->Initialize(&renderConfig, pRenderWindow);

	MC::MCD3D::Instance()->Initialize(renderConfig);

	pRenderWindow->RegisterResizeCallback(
		[]() {
			MC::MCDXGI::Instance()->QueueResize();
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

			MC::MCD3D::Instance()->RenderFrame(&frame);

			frameCount++;

			

			float fps = frameCount / masterTimer->TotalTime();

			std::string t = std::string("MCDemo Frame(") + std::to_string(frameCount) + std::string(") fps: ") + std::to_string(fps);

			SetWindowTextA(pRenderWindow->hWnd(), t.c_str());

		}
	}
	
	return 0;
}

std::atomic_int t2(0);

void foo() {

	int t = 0;
	t++;
}

void sum(__int64 *pInt, int numSums, std::atomic_int *pAtomicInt){
	for (int c = 0; c < numSums; ++c) {
		ENTER_CRITICAL_SECTION(CS, pAtomicInt);
		*pInt = *pInt + 1;
		ENTER_CRITICAL_SECTION(bad, &t2);
		EXIT_CRITICAL_SECTION
		EXIT_CRITICAL_SECTION
	}
}

const int NUM_SUMS = 5;
const int THREADS_PER_SUM = 5;
void CriticalSectionTest() {
	__int64 sums[NUM_SUMS] = {};
	MC::MCCriticalSectionLock locks[NUM_SUMS];
	std::thread *threads[NUM_SUMS][THREADS_PER_SUM];

	for (int x = 0; x < NUM_SUMS; ++x) {
		MC::MCCriticalSection::InitializeLock(&locks[x]);
		for (int y = 0; y < THREADS_PER_SUM; ++y) {
			threads[x][y] = new std::thread(sum, &sums[x], 100000, &locks[x]);
		}
	}

	for (int x = 0; x < NUM_SUMS; ++x) {
		for (int y = 0; y < THREADS_PER_SUM; ++y) {
			threads[x][y]->join();
		}
	}

	for (int x = 0; x < NUM_SUMS; ++x)
		std::cout << "sum #" << x << " = " << sums[x] << std::endl;
}

int main(int argc, char ** argv) {
	try {
		Sandbox();
		/*for (int x = 0; x < 100; ++x)
			CriticalSectionTest();
		char t;
		std::cin >> t;*/
	}
	catch (MCException *ex) {
		MessageBox(nullptr, ex->what().c_str(), "MCException", MB_OK);
	}
	catch (MCException ex) {
		MessageBox(nullptr, ex.what().c_str(), "MCException", MB_OK);
	}

	MC::MCD3D12RenderUtilities::OrderedStaticDestroy();
}
