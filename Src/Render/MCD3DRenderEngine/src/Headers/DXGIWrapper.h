#pragma once

#include "dxgi1_6.h"
#include "d3d12.h"
#include "wrl.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class DXGIWrapper {
	public:
		static DXGIWrapper &GetInstance() {
			static DXGIWrapper instance;
			return instance;
		}
		~DXGIWrapper();

	public:
		void Init();
		
		IDXGIAdapter *GetConfiguredAdapter();
		IDXGIOutput  *GetConfiguredOutput();

	private:
		DXGIWrapper();
		DXGIWrapper(DXGIWrapper&)  = delete;
		DXGIWrapper(DXGIWrapper&&) = delete;

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter *pAdapter);
		void LogOutputDisplayModes(IDXGIOutput *pOutput, DXGI_FORMAT format);
		void InitDXGIFactory();

		ComPtr<IDXGIFactory4> _pDXGIFactory;
		ComPtr<ID3D12Device>  _pD3DDevice;

	};

}
