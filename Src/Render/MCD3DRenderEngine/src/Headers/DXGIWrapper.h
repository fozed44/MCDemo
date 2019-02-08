#pragma once

#include "dxgi1_6.h"
#include "d3d12.h"
#include "wrl.h"
#include "RenderConfig.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class DXGIWrapper {
	public:
		DXGIWrapper();
		~DXGIWrapper();

	public:
		void Init(const RENDER_CONFIG* pConfig);
		
		inline bool Initialized() { return _initialized; }

		
		ID3D12Device *CreateConfiguredOrDefault3DDevice();
		ID3D12Device *Get3DDevice();
		
		IDXGISwapChain3 *CreateConfiguredOrDefaltSwapchain(ID3D12CommandQueue *pCommandQueue);
		IDXGISwapChain3 *GetSwapChain();

		IDXGIAdapter *GetConfiguredOrDefaultAdapter();
		IDXGIOutput  *GetConfiguredOrDefaultOutput();

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

		void EnableDXDebugLayer();

		bool _initialized;
		const RENDER_CONFIG _initialConfiguration;

		ComPtr<IDXGIFactory4>   _pDXGIFactory;
		ComPtr<ID3D12Device>    _p3DDevice;
		ComPtr<IDXGISwapChain3> _pSwapchain;
	};

}
