#pragma once

#include "Windows.h"
#include "RenderConfig.h"
namespace MC {

	class WindowWrapper {
	public:
		WindowWrapper(const RENDER_CONFIG& renderConfig);
		WindowWrapper(const WindowWrapper&)            = delete;
		WindowWrapper(const WindowWrapper&&)           = delete;
		WindowWrapper& operator=(const WindowWrapper&) = delete;
		~WindowWrapper();

		HWND Init();
		inline bool Initialized() { return _initialized; }
		inline HWND hWnd()        { return _hwnd; }

	private:

		/*****************************************************/
		/* INITIALIZATION HELPERS                            */
		/*****************************************************/
		void RegisterMCWindowClass();
		void CreateMCWindow();
		void EnsureValidWindowConfig();

	private:
		bool                _initialized;
		HWND                _hwnd;
		const RENDER_CONFIG _renderConfig;
	};

}