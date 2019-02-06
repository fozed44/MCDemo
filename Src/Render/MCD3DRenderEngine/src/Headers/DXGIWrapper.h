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

		IDXGIAdapter *GetConfiguredOrDefaultAdapter();
		IDXGIOutput  *GetConfiguredOutput();

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

		bool _initialized;
		const RENDER_CONFIG _initialConfiguration;

		ComPtr<IDXGIFactory4> _pDXGIFactory;
		ComPtr<ID3D12Device>  _pD3DDevice;
	};

}
