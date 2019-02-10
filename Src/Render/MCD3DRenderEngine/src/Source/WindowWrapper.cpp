#include "../Headers/WindowWrapper.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

#define MC_WINDOW_CLASS_NAME "MCWindow"
#define MC_WINDOW_TITLE "MC Demo"

// Used for error detection.
#define MAX_VALID_WINDOW_WIDTH  10000
#define MAX_VALID_WINDOW_HEIGHT 10000

namespace MC {

#pragma region window_proc

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_LBUTTONDOWN:
			MC_INFO("Mouse button down at {0:d}, {1:d}", LOWORD(lParam), HIWORD(lParam));
			break;
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

	/*
		Register and create a render window.
	*/
	HWND WindowWrapper::Init() {
		RegisterMCWindowClass();
		CreateMCWindow();		

		ShowWindow(_hwnd, SW_RESTORE);
		UpdateWindow(_hwnd);
		
		_initialized = true;

		return _hwnd;
	}

	/*
		Registers the actual rendering window.
	*/
	void WindowWrapper::RegisterMCWindowClass() {
		WNDCLASS wc;

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = MC_WINDOW_CLASS_NAME;

		INIT_INFO("Registering window class.");

		if (!RegisterClass(&wc)) {
			INIT_ERROR("Failed to register window class!");
			MCTHROW("Failed to register window class!");
		}

		INIT_INFO("Registered.");
	}

	/*
		Create the actual rendering window.
	*/
	void WindowWrapper::CreateMCWindow() {
		INIT_INFO("Creating window.");

		EnsureValidWindowConfig();

		_hwnd = CreateWindow(
			MC_WINDOW_CLASS_NAME,                // Class name
			MC_WINDOW_TITLE,                     // Window title
			WS_OVERLAPPEDWINDOW,                 // style flags
			CW_USEDEFAULT,                       // x-coordinate
			CW_USEDEFAULT,                       // y-coordinate
			_renderConfig.DISPLAY_OUTPUT_WIDTH,  // width
			_renderConfig.DISPLAY_OUTPUT_HEIGHT, // height
			0,                                   // parent window
			0,                                   // menu handle
			0,                                   // app instance
			0                                    // extra creation parameters
		);

		if (_hwnd == 0) {
			INIT_ERROR("Failed to create window!");
			MCTHROW("Failed to create window!");
		}

		INIT_INFO("Window created.");
	}

	/*
		Examine the window width and height in _renderConfig. Throw an exception if the values do not
		fall within an acceptable range.
	*/
	void WindowWrapper::EnsureValidWindowConfig() {
		if (_renderConfig.DISPLAY_OUTPUT_WIDTH <= 0
			|| _renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_WIDTH) {
			INIT_ERROR("Detected an invalid window width ({0:d}) in the config file.", _renderConfig.DISPLAY_OUTPUT_WIDTH);
			MCTHROW("Detected an invalid window width in the config file.");
		}

		if (_renderConfig.DISPLAY_OUTPUT_HEIGHT <= 0
			|| _renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_HEIGHT) {
			INIT_ERROR("Detected an invalid window height ({0:d}) in the config file.", _renderConfig.DISPLAY_OUTPUT_HEIGHT);
			MCTHROW("Detected an invalid window height in the config file.");
		}
	}


#pragma endregion

}