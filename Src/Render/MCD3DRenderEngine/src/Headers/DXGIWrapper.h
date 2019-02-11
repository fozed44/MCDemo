#pragma once

#include "dxgi1_6.h"
#include "d3d12.h"
#include "wrl.h"
#include <memory>

#include "MCD3DRenderEngineConstants.h"
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

		bool _initialized;
		const RENDER_CONFIG _initialConfiguration;
		std::shared_ptr<WindowWrapper> _pWindowWrapper;

		ComPtr<IDXGIFactory4>   _pDXGIFactory;
		ComPtr<ID3D12Device>    _p3DDevice;
		ComPtr<IDXGISwapChain3> _pSwapChain;
	};

}
