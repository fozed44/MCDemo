#define __ENABLE_INIT_TRACE

#include "../Headers/DXGIWrapper.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/Headers/Common.h"

namespace MC {

	DXGIWrapper::DXGIWrapper() {}

	DXGIWrapper::~DXGIWrapper(){}

	void DXGIWrapper::Init() {
		InitDXGIFactory();
		LogAdapters();
	}

	void DXGIWrapper::LogAdapters() {
		int i = 0;
		IDXGIAdapter* pAdapter = nullptr;
		std::vector<IDXGIAdapter*> adapterList;

		while (_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
			DXGI_ADAPTER_DESC desc;

			pAdapter->GetDesc(&desc);

			std::wstring adapterDescW(desc.Description);
			std::string adapterDesc(adapterDescW.begin(), adapterDescW.end());

			INIT_INFO("***Adapter: {0}", adapterDesc.c_str());
			INIT_INFO("  Device ID: {0:d}", desc.DeviceId);

			LogAdapterOutputs(pAdapter);

			SAFE_RELEASE(pAdapter);
			++i;
		}

	}

	void DXGIWrapper::LogAdapterOutputs(IDXGIAdapter* pAdapter) {
		UINT i = 0;
		IDXGIOutput *pOutput = nullptr;

		while (pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND) {
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);

			std::wstring deviceNameW(desc.DeviceName);
			std::string deviceName(deviceNameW.begin(), deviceNameW.end());

			INIT_INFO("   ***Output Device: {0}", deviceName);

			LogOutputDisplayModes(pOutput, DXGI_FORMAT_R8G8B8A8_UNORM);

			SAFE_RELEASE(pOutput);
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

			std::wstring logOutW =
				L"{ " + std::to_wstring(x.Width) + L" x " + std::to_wstring(x.Height) + L" } -- Refresh: " +
				std::to_wstring(n) + L"/" + std::to_wstring(d);
			std::string logOut(logOutW.begin(), logOutW.end());

			INIT_INFO("      ***{0}", logOut.c_str());			
		}
	}

	void DXGIWrapper::InitDXGIFactory() {
		UINT dxgiCreateFlags = 0;

		INIT_TRACE("Creating DXGIFactory.");
		ThrowIfFailed(CreateDXGIFactory2(dxgiCreateFlags, __uuidof(_pDXGIFactory), &_pDXGIFactory));
		INIT_TRACE("Success.");
	}

}