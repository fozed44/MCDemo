#pragma once

#include "MCD3D12Core.h"
#include "../Configuration/RenderConfig.h"

#ifdef _DEBUG

#define DUMP_LIVE_OBJECTS_DETAIL MC::MCREGlobals::pMCDXGI->DumpLiveObjects(D3D12_RLDO_DETAIL)
#define DUMP_LIVE_OBJECTS_SUMMARY MC::MCREGlobals::pMCDXGI->DumpLiveObjects(D3D12_RLDO_SUMMARY)

#else

#define DUMP_LIVE_OBJECTS_DETAIL
#define DUMP_LIVE_OBJECTS_SUMMARY

#endif _DEBUG

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCDXGI {
	public:
		MCDXGI();
		/*	Simple destructor - MCDXGI owns no heap memory. */
		~MCDXGI();
		MCDXGI(MCDXGI&)              = delete;
		MCDXGI(MCDXGI&&)             = delete;
		MCDXGI& operator= (MCDXGI&)  = delete;
		MCDXGI& operator= (MCDXGI&&) = delete;

	public:
		
		/*	Creates a new device based on the configuration and the render window supplied to the Initialize method.

			NOTE:
				CreateConfiguredOrDefault3DDevice must be called in order to create the internal ID3D12Device. Other
				methods will fail if this method has not been called.
				
				Further, CreateConfiguredOrDefault3DDeviceonly must be called once and only once. */
		ID3D12Device *CreateConfiguredOrDefault3DDevice();

		/*	Creates a swap chain for the given ID3D12CommandQueue. */
		IDXGISwapChain3 *CreateConfiguredOrDefaltSwapchain(ID3D12CommandQueue *pCommandQueue);

		/*	Access the ID3DDevice previously created by the call to CreateConfiguredOrDefault3DDevice. */
		ID3D12Device *Get3DDevice();
		

		/*	Internally calls present on the swap-chain.

			Note:
				After calling Present. The frame at the index previously returned from GetCurrentBackBufferIndex
				will now be drawn to the screen and GetCurrentBackBufferIndex will return a new index. */
		HRESULT Present();		

		/*	Query the memory info the adapter associated with the current ID3D12Device.	*/
		void QueryLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const;
		void QueryNonLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const;

		/*	Returns the index of the current back buffer that is being presented by the DXGI. */
		unsigned int GetCurrentBackBufferIndex();

		/*	Return the index of the current buffer that is being presented to the screen. */
		unsigned int GetCurrentPresentedBufferIndex();

		/*	Return the swap chain buffer at position pos. */
		void GetFrameBuffer(UINT pos, const IID &riid, void **ppSurface);

		/*	Returns that last calculated aspect ratio. This cached value is updated when the swap chain
			is either created or resized. */
		inline float GetAspectRatio() { return _cachedAspectRatio; }

		/*	It is intended that QueueResize can be called from the thread running the message pump.The
			render thread can then call IsResizeQueued and ForceResize to cause the actual resizing to be
			controlled by the renderer. */
		inline void QueueResize()     { _resizeQueued = true; }
		inline bool IsResizeQueued()  { return _resizeQueued; }
		       void ForceResize();

		/*	Get the dimensions of (one of) the frame buffer. The number of buffers actually being used by
			the swapchain is not relevant because they all must be the same size. */
		void GetFrameBufferSize(int *pWidth, int *pHeight);

		/* In Debug builds we can call DumpLiveObjects to send the live objects on the device to the output window. 
		   Further, even in debug builds, the device needs to be created with the debug layer enabled.
		   NOTE: 
			Don't call this method directly, instead use the DUMP_LIVE_OBJECTS macro that will disappear in non-debug
			builds.*/
#ifdef _DEBUG
		void DumpLiveObjects(D3D12_RLDO_FLAGS flags);
#endif _DEBUG
	private:
		IDXGIAdapter *CreateConfiguredOrDefaultAdapter();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter *pAdapter);
		IDXGIAdapter *GetAdapterByDeviceID(UINT deviceId);
		IDXGIAdapter *GetDefaultAdapter();
		void LogOutputDisplayModes(IDXGIOutput *pOutput, DXGI_FORMAT format);
		void InitDXGIFactory();
		void EnsureValidWindowConfig(const RENDER_CONFIG& renderConfig) const;

		void EnableDXDebugLayer();
		
		/*
			Updated when a swapchain is created or resized.
		*/
		float _cachedAspectRatio;

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
		ComPtr<IDXGIAdapter3>   _pAdapter;
	};

}
