#pragma once

#include "MCD3D12Core.h"
#include "DirectXMath.h"
#include "RenderConfig.h"
#include <functional>
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

		void RegisterResizeCallback(std::function<void()> callback) const;

		// Temp Methods that will be deleted
		float GetPhi()    { return _phi; }
		float GetTheta()  { return _theta; }
		float GetRadius() { return _radius; }

		static float _phi;
		static float _theta;
		static float _radius;

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