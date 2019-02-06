#define __ENABLE_INIT_TRACE

#include "../Headers/DXGIWrapper.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

namespace MC {

	DXGIWrapper::DXGIWrapper() 
		: _initialized(false) {}

	DXGIWrapper::~DXGIWrapper(){}

	void DXGIWrapper::Init(const RENDER_CONFIG *pConfig) {

		MC_INFO("Begin DXGI initialization.");

		InitConfiguration(pConfig);
		InitDXGIFactory();
		LogAdapters();

		_initialized = true;

		MC_INFO("End DXGI initialization.");
	}

	void DXGIWrapper::InitConfiguration(const RENDER_CONFIG *pConfig) {
		const_cast<RENDER_CONFIG&>(_initialConfiguration) = *pConfig;
	}

	void DXGIWrapper::LogAdapters() {
		int i = 0;
		IDXGIAdapter* pAdapter = nullptr;

		while (_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC desc;

			MCThrowIfFailed(pAdapter->GetDesc(&desc));

			std::string adapterDesc(ws2s(desc.Description));

			INIT_INFO("***Adapter: {0}", adapterDesc.c_str());
			INIT_INFO("  Device ID: {0:d}", desc.DeviceId);

			LogAdapterOutputs(pAdapter);

			MCSAFE_RELEASE(pAdapter);
			++i;
		}
	}
	
	void DXGIWrapper::LogAdapterOutputs(IDXGIAdapter* pAdapter) {
		UINT i = 0;
		IDXGIOutput *pOutput = nullptr;

		while (pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND) {
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);

			std::string deviceName(ws2s(desc.DeviceName));

			INIT_INFO("   ***Output Device: {0}", deviceName);

			LogOutputDisplayModes(pOutput, DXGI_FORMAT_R8G8B8A8_UNORM);

			MCSAFE_RELEASE(pOutput);
			++i;
		}

	}

	void DXGIWrapper::LogOutputDisplayModes(IDXGIOutput* pOutput, DXGI_FORMAT format) {
		UINT count = 0;
		UINT flags = 0;

		pOutput->GetDisplayModeList(format, flags, &count, nullptr);

		if (count == 0) {
			INIT_INFO("      ***No outputs for device.");
			return;
		}

		std::vector<DXGI_MODE_DESC> modeList(count);
		pOutput->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& x : modeList) {
			UINT n = x.RefreshRate.Numerator;
			UINT d = x.RefreshRate.Denominator;

			std::string logOut =
				"{ " + std::to_string(x.Width) + " x " + std::to_string(x.Height) + " } -- Refresh: " +
				std::to_string(n) + "/" + std::to_string(d);

			INIT_INFO("      ***{0}", logOut.c_str());			
		}
	}

	void DXGIWrapper::InitDXGIFactory() {
		UINT dxgiCreateFlags = 0;

		INIT_TRACE("Creating DXGIFactory.");
		MCThrowIfFailed(CreateDXGIFactory2(dxgiCreateFlags, __uuidof(_pDXGIFactory), &_pDXGIFactory));
		INIT_TRACE("Success.");
	}

	IDXGIAdapter *DXGIWrapper::GetAdapterByDeviceID(UINT deviceId) {
		int i = 0;
		IDXGIAdapter* pAdapter = nullptr;

		while (_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC desc;

			MCThrowIfFailed(pAdapter->GetDesc(&desc));

			if (desc.DeviceId == deviceId)
				return pAdapter;

			MCSAFE_RELEASE(pAdapter);
			++i;
		}

		return nullptr;
	}

	IDXGIAdapter *DXGIWrapper::GetDefaultAdapter() {
		int i = 0;
		IDXGIAdapter1* pAdapter = nullptr;

		while (_pDXGIFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC1 desc;

			MCThrowIfFailed(pAdapter->GetDesc1(&desc));

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {

				INIT_INFO("Returning device {0:d} as default.", desc.DeviceId);

				return pAdapter;
			}

			MCSAFE_RELEASE(pAdapter);
			++i;
		}

		return nullptr;
	}

	IDXGIAdapter *DXGIWrapper::GetConfiguredOrDefaultAdapter() {
		INIT_INFO("Attempting to get configured adapter: {0:d}", _initialConfiguration.ADAPTER_DEVICE_ID);
		auto configuredAdapter = GetAdapterByDeviceID(_initialConfiguration.ADAPTER_DEVICE_ID);

		if (configuredAdapter != nullptr) {
			INIT_INFO("Atapter found.");
			return configuredAdapter;
		}

		INIT_INFO("Failed to locate adapter: {0:d}", _initialConfiguration.ADAPTER_DEVICE_ID);
		INIT_INFO("Loading default adapter");

		auto defaultAdapter = GetDefaultAdapter();

		if (defaultAdapter == nullptr) {
			MCTHROW("Failed to load default dxgi adapter.");
		}

		INIT_INFO("Loaded default adapter");

		return defaultAdapter;
	}

}