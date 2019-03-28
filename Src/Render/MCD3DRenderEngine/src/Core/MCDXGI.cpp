#include "../../../../Common/MCCommon/src/Headers/GlobalSwitches.h"

#include "MCDXGI.h"
#include "MCMath.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "MCREGlobals.h"
#include "MCRenderWindow.h"

namespace MC {

	MCDXGI::MCDXGI() : _cachedAspectRatio{ 0 } {
		assert(MCREGlobals::pRenderWindow);

		MC_INFO("Begin DXGI initialization.");

		InitDXGIFactory();
		LogAdapters();

		MC_INFO("End DXGI initialization.");
	}

	MCDXGI::~MCDXGI(){}

	void MCDXGI::LogAdapters() {
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
	
	void MCDXGI::LogAdapterOutputs(IDXGIAdapter* pAdapter) {
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

	void MCDXGI::LogOutputDisplayModes(IDXGIOutput* pOutput, DXGI_FORMAT format) {
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

	void MCDXGI::InitDXGIFactory() {
		UINT dxgiCreateFlags = 0;

		// Note:
		//	This debug layer is enabled via the _initialCOnfiguration.DEBUG_ENABLE_DX_DEBUG flag
		//  this flag also triggers CreateConfiguredOrDefault3DDevice to call EnableDebugLayer before
		//  creating a new device.
		if (MCREGlobals::InitialConfiguration.DEBUG_ENABLE_DX_DEBUG) {
			INIT_INFO("Enabling debug for dxgi factory.");
			dxgiCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		INIT_TRACE("Creating DXGIFactory.");
		MCThrowIfFailed(CreateDXGIFactory2(dxgiCreateFlags, __uuidof(_pDXGIFactory), &_pDXGIFactory));
		INIT_TRACE("Success.");
	}

	IDXGIAdapter *MCDXGI::GetAdapterByDeviceID(UINT deviceId) {
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

	IDXGIAdapter *MCDXGI::GetDefaultAdapter() {
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

	IDXGIAdapter *MCDXGI::CreateConfiguredOrDefaultAdapter() {
		assert(_pAdapter == nullptr);
		ComPtr<IDXGIAdapter> tempAdapter;
		INIT_INFO("Attempting to get configured adapter: {0:d}", MCREGlobals::InitialConfiguration.DISPLAY_ADAPTER_DEVICE_ID);
		auto configuredAdapter = GetAdapterByDeviceID(MCREGlobals::InitialConfiguration.DISPLAY_ADAPTER_DEVICE_ID);

		if (configuredAdapter != nullptr) {
			INIT_INFO("Adapter found.");
			tempAdapter.Attach(configuredAdapter);
			MCThrowIfFailed(tempAdapter.As(&_pAdapter));
			return configuredAdapter;
		}

		INIT_INFO("Failed to locate adapter: {0:d}", MCREGlobals::InitialConfiguration.DISPLAY_ADAPTER_DEVICE_ID);
		INIT_INFO("Loading default adapter");

		auto defaultAdapter = GetDefaultAdapter();

		if (defaultAdapter == nullptr) {
			MCTHROW("Failed to load default DXGI adapter.");
		}

		tempAdapter.Attach(defaultAdapter);
		MCThrowIfFailed(tempAdapter.As(&_pAdapter));
		tempAdapter.Detach();

		INIT_INFO("Loaded default adapter");

		return defaultAdapter;
	}

	ID3D12Device *MCDXGI::CreateConfiguredOrDefault3DDevice() {

		// If there is already a device in existence, release it.
		MCSAFE_RELEASE(_p3DDevice);

		// Note:
		//	This debug layer is enabled via the _initialCOnfiguration.DEBUG_ENABLE_DX_DEBUG flag
		//  this flag also triggers CreateConfiguredOrDefault3DDevice to call EnableDebugLayer before
		//  creating a new device.
		if (MCREGlobals::InitialConfiguration.DEBUG_ENABLE_DX_DEBUG) {
			INIT_INFO("Enabling debug for d3d12 device.");
			EnableDXDebugLayer();
		}

		// TODO:
		//	We don't really want to be messing with raw com pointers that need to be released.
		//  FIND AND USE A BETTER SOLUTION.
		auto pAdapter = CreateConfiguredOrDefaultAdapter();

		INIT_INFO("Creating 3D Device.");

		MCThrowIfFailed(D3D12CreateDevice(
			(IUnknown*)pAdapter,
			D3D_FEATURE_LEVEL_11_0,
			__uuidof(_p3DDevice),
			&_p3DDevice
		));

		INIT_INFO("Device successfully created.");

		MCSAFE_RELEASE(pAdapter);

		return _p3DDevice.Get();
	}

	ID3D12Device *MCDXGI::Get3DDevice() {
		assert(_p3DDevice);
		return _p3DDevice.Get();
	}

	void MCDXGI::EnableDXDebugLayer() {
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))){
			debugController->EnableDebugLayer();
		}
	}

	IDXGISwapChain3 *MCDXGI::CreateConfiguredOrDefaltSwapchain(ID3D12CommandQueue *pCommandQueue) {

		// Ensure that the window configuration is valid... We will be using the dimensions of the window
		// as the dimensions of the back buffer.
		EnsureValidWindowConfig(MCREGlobals::InitialConfiguration);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
		swapChainDesc.Width       = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_WIDTH;
		swapChainDesc.Height      = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_HEIGHT;
		swapChainDesc.Format      = BACK_BUFFER_FORMAT;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		// TODO:
		//	Add tearing support.

		ComPtr<IDXGISwapChain1> swapChain;
		
		assert(MCREGlobals::pRenderWindow->hWnd());

		MCThrowIfFailed(_pDXGIFactory->CreateSwapChainForHwnd(
			pCommandQueue,
			MCREGlobals::pRenderWindow->hWnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));
		
		MCThrowIfFailed(swapChain.As(&_pSwapChain));

		// Update the cached aspect ratio.
		_cachedAspectRatio = static_cast<float>(swapChainDesc.Width) / static_cast<float>(swapChainDesc.Height);

		return _pSwapChain.Get();
	}

	/*
	Examine the window width and height in _renderConfig. Throw an exception if the values do not
	fall within an acceptable range.
	*/
	void MCDXGI::EnsureValidWindowConfig(const RENDER_CONFIG& renderConfig) const {
		if (renderConfig.DISPLAY_OUTPUT_WIDTH <= 0
			|| renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_WIDTH) {
			INIT_ERROR("Detected an invalid window width ({0:d}) in the config file.", renderConfig.DISPLAY_OUTPUT_WIDTH);
			MCTHROW("Detected an invalid window width in the config file.");
		}

		if (renderConfig.DISPLAY_OUTPUT_HEIGHT <= 0
			|| renderConfig.DISPLAY_OUTPUT_WIDTH > MAX_VALID_WINDOW_HEIGHT) {
			INIT_ERROR("Detected an invalid window height ({0:d}) in the config file.", renderConfig.DISPLAY_OUTPUT_HEIGHT);
			MCTHROW("Detected an invalid window height in the config file.");
		}
	}

	unsigned int MCDXGI::GetCurrentBackBufferIndex() {
		assert(_pSwapChain);
		return _pSwapChain->GetCurrentBackBufferIndex();
	}

	void MCDXGI::GetFrameBuffer(UINT pos, const IID &riid, void **ppSurface) {
		MCThrowIfFailed(_pSwapChain->GetBuffer(pos, riid, ppSurface));
	}

	HRESULT MCDXGI::Present() {
		return _pSwapChain->Present(1, 0);
	}

	void MCDXGI::ForceResize() {
		MCThrowIfFailed(_pSwapChain->ResizeBuffers(
			FRAME_BUFFER_COUNT,
			0,
			0,
			BACK_BUFFER_FORMAT,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		));

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		MCThrowIfFailed(_pSwapChain->GetDesc(&swapChainDesc));

		_cachedAspectRatio =
			static_cast<float>(swapChainDesc.BufferDesc.Width)
		  / static_cast<float>(swapChainDesc.BufferDesc.Height);

		_resizeQueued = false;
	}

	void MCDXGI::GetFrameBufferSize(int *pWidth, int *pHeight) {
		assert(_pSwapChain);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		MCThrowIfFailed(_pSwapChain->GetDesc(&swapChainDesc));

		*pWidth  = swapChainDesc.BufferDesc.Width;
		*pHeight = swapChainDesc.BufferDesc.Height;
	}

	void MCDXGI::QueryLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const {
		assert(_pAdapter);
		_pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, pMemoryInfo);
	}

	void MCDXGI::QueryNonLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const {
		assert(_pAdapter);
		_pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, pMemoryInfo);
	}
}