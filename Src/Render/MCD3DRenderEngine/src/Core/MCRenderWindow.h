#pragma once

#include "MCD3D12Core.h"
#include "DirectXMath.h"
#include "../Configuration/RenderConfig.h"

#include <functional>
namespace MC {

	class MCRenderWindow {
	public:
		MCRenderWindow();
		MCRenderWindow(const MCRenderWindow&)            = delete;
		MCRenderWindow(const MCRenderWindow&&)           = delete;
		MCRenderWindow& operator=(const MCRenderWindow&) = delete;
		~MCRenderWindow();

		inline HWND hWnd() { return _hwnd; }

		void RegisterResizeCallback(std::function<void()> callback) const;
		void RegisterKeyDownCallback(std::function<void(uint8_t)> callback) const;

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
		void EnsureValidWindowConfig(const RENDER_CONFIG& renderConfig) const;

	private:
		HWND _hwnd;
	};

}