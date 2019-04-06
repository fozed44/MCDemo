#include "../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCDXGI.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCD3D.h"
#include "../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../Render/MCD3DRenderEngine/Src/Core/MCRenderWindow.h"
#include "../../../Common/MCCommon/src/Headers/MasterTimer.h"
#include "../../../Common/MCCommon/src/Data/MCFrame.h"
#include "../../../Common/MCCommon/src/Headers/MCCriticalSection.h"
#include "../../../Render/MCD3DRenderEngine/src/Memory/MCResourceManager.h"
#include "../../../Render/MCD3DRenderEngine/src/Configuration/RenderConfig.h"
#include "../../../Render/MCD3DRenderEngine/src/Core/MCD3D12RenderEngine.h"
#include "../../../Render/MCD3DRenderEngine/src/Core/MCREGlobals.h"
#include "../../../Common/MCCommon/src/Data/MCFrame.h"
#include "../../../Common/MCCommon/src/Data/MCResult.h"
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

	auto pRenderEngine = std::make_unique<MC::MCD3D12RenderEngine>(&renderConfig);

	pRenderEngine->SetRenderState(MC::MC_RENDER_STATE_SPACE);

	MC::MCREGlobals::pRenderWindow->RegisterResizeCallback(
		[]() {
			MC::MCREGlobals::pMCDXGI->QueueResize();
		}
	);

	MSG msg = {};
	
	__int64 frameCount = 1;

	MC::MCFrame3D frame;
	frame.Camera.Position = { 0.0f, 0.0f, -10.0f };
	frame.Camera.LookAt   = { 0.0f, 0.0f, 0.0f };
	
	const __int64 frameCountBufferSize = 50;
	float frameCountBuffer[frameCountBufferSize];
	MC::MCSpaceFrame *pNextFrame = new MC::MCSpaceFrame();
	pNextFrame->FrameType = MC::MC_FRAME_TYPE_MCFRAME_SPACE;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			masterTimer->Tick();

			frame.Camera.Position.x = MC::MCREGlobals::pRenderWindow->GetRadius()*sinf(MC::MCREGlobals::pRenderWindow->GetPhi())*cosf(MC::MCREGlobals::pRenderWindow->GetTheta());
			frame.Camera.Position.z = MC::MCREGlobals::pRenderWindow->GetRadius()*sinf(MC::MCREGlobals::pRenderWindow->GetPhi())*sinf(MC::MCREGlobals::pRenderWindow->GetTheta());
			frame.Camera.Position.y = MC::MCREGlobals::pRenderWindow->GetRadius()*cosf(MC::MCREGlobals::pRenderWindow->GetPhi());
			frame.Time = masterTimer->TotalTime();

			//MC::MCREGlobals::pMCD3D->RenderFrame(&frame);
			if (MC::MC_RESULT_OK == MC::MCREGlobals::pRenderEngine->ScheduleFrame(pNextFrame)) {
				pNextFrame = new MC::MCSpaceFrame();
				pNextFrame->FrameType = MC::MC_FRAME_TYPE_MCFRAME_SPACE;
			}

			MC::MCREGlobals::pRenderEngine->Update();

			frameCount++;

			frameCountBuffer[frameCount % frameCountBufferSize] = masterTimer->TotalTime();

			float fps = 1.0f / ((frameCountBuffer[frameCount % frameCountBufferSize] - frameCountBuffer[(frameCount + 1) % frameCountBufferSize]) /
				((float)frameCountBufferSize));

			std::string t = std::string("MCDemo Frame(") + std::to_string(frameCount) + std::string(") fps: ") + std::to_string(fps);

			SetWindowTextA(MC::MCREGlobals::pRenderWindow->hWnd(), t.c_str());

			std::this_thread::sleep_for(std::chrono::milliseconds(3));

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

}
