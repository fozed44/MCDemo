#pragma once

#include "MCD3D12Core.h"
#include "DirectXMath.h"
#include "RenderConfig.h"
#include <functional>
namespace MC {

	class MCRenderWindow {
	public:
		MCRenderWindow(const RENDER_CONFIG& renderConfig);
		MCRenderWindow(const MCRenderWindow&)            = delete;
		MCRenderWindow(const MCRenderWindow&&)           = delete;
		MCRenderWindow& operator=(const MCRenderWindow&) = delete;
		~MCRenderWindow();

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