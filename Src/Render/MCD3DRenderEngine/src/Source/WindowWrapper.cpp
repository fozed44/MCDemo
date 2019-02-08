#include "../Headers/WindowWrapper.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

namespace MC {


#pragma region window_proc

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

#pragma endregion

#pragma region ctor

	WindowWrapper::WindowWrapper(const RENDER_CONFIG& renderConfig)
		: _initialized(false), _hwnd(0), _renderConfig(renderConfig) {}

	WindowWrapper::~WindowWrapper() {}

#pragma endregion

#pragma region Initialization

	HWND WindowWrapper::Init() {
		// TODO:
		//	This method needs to use _initialRenderConfig to make the window creation code
		//  use the configuration options.
		WNDCLASS wc;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = NULL;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = "MCWindowClass";

		INIT_INFO("Registering window class.");

		if (!RegisterClass(&wc)) {
			INIT_ERROR("Failed to register window class!");
			MCTHROW("Failed to register window class!");
		}

		INIT_INFO("Registered.");

		INIT_INFO("Creating window.");

		_hwnd = CreateWindow(
			"MCWindowClass",     // Class name
			"MC",                // Window title
			WS_OVERLAPPEDWINDOW, // style flags
			CW_USEDEFAULT,       // x-coordinate
			CW_USEDEFAULT,       // y-coordinate
			CW_USEDEFAULT,       // width
			CW_USEDEFAULT,       // height
			0,                   // parent window
			0,                   // menu handle
			0,                   // app instance
			0                    // extra creation parameters
		);

		if (_hwnd == 0) {
			INIT_ERROR("Failed to create window!");
			MCTHROW("Failed to create window!");
		}
		
		INIT_INFO("Window created.");

		ShowWindow(_hwnd, SW_RESTORE);
		UpdateWindow(_hwnd);

		return _hwnd;
	}

#pragma endregion

}