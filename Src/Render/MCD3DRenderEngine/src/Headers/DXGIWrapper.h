#pragma once


#include "MCD3D12Core.h"
#include "RenderConfig.h"
#include "WindowWrapper.h"


using Microsoft::WRL::ComPtr;

namespace MC {

	class DXGIWrapper {
	public:
		DXGIWrapper();
		~DXGIWrapper();

	public:
		void Init(const RENDER_CONFIG* pConfig, std::shared_ptr<WindowWrapper>& windowWrapper);
		
		inline bool Initialized() { return _initialized; }
		
		ID3D12Device *CreateConfiguredOrDefault3DDevice();
		ID3D12Device *Get3DDevice();
		
		IDXGISwapChain3 *CreateConfiguredOrDefaltSwapchain(ID3D12CommandQueue *pCommandQueue);
		IDXGISwapChain3 *GetSwapChain();
		HRESULT Swap();

		IDXGIAdapter *GetConfiguredOrDefaultAdapter();
		IDXGIOutput  *GetConfiguredOrDefaultOutput();

		/*
			Returns the index of the current back buffer that is being presented by the DXGI.
		*/
		unsigned int GetCurrentBackBufferIndex();

		/*
			Return the swap chain buffer at position pos.
		*/
		void GetFrameBuffer(UINT pos, const IID &riid, void **ppSurface);

		/*
			Returns that last calculated aspect ratio. This cached value is updated when the swap chain
			is either created or resized.
		*/
		inline float GetAspectRatio() { return _cachedAspectRatio; }

		/*
			It is intended that QueueResize can be called from the thread running the message pump.The
			render thread can then call IsResizeQueued and ForceResize to cause the actual resizing to be
			controlled by the renderer.
		*/
		inline void QueueResize()     { _resizeQueued = true; }
		inline bool IsResizeQueued()  { return _resizeQueued; }
		       void ForceResize();

	private:
		DXGIWrapper(DXGIWrapper&)  = delete;
		DXGIWrapper(DXGIWrapper&&) = delete;

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter *pAdapter);
		IDXGIAdapter *GetAdapterByDeviceID(UINT deviceId);
		IDXGIAdapter *GetDefaultAdapter();
		void LogOutputDisplayModes(IDXGIOutput *pOutput, DXGI_FORMAT format);
		void InitDXGIFactory();
		void InitConfiguration(const RENDER_CONFIG *pConfig);
		void EnsureValidWindowConfig();

		void EnableDXDebugLayer();

		bool                           _initialized;
		const RENDER_CONFIG            _initialConfiguration;
		std::shared_ptr<WindowWrapper> _pWindowWrapper;

		/*
			Updated when a swapchain is created or resized.
		*/
		float                          _cachedAspectRatio;

		/*
			Set to true by the QueueResize method.

			It is intended that QueueResize can be called from the thread running the message pump. The
			render thread can then call IsResizeQueued and ForceResize to cause the actual resizing to be
			controlled by the renderer.
		*/
		bool _resizeQueued;

		ComPtr<IDXGIFactory4>   _pDXGIFactory;
		ComPtr<ID3D12Device>    _p3DDevice;
		ComPtr<IDXGISwapChain3> _pSwapChain;
	};

}
