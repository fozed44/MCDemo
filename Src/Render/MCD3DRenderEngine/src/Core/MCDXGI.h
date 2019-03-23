#pragma once


#include "MCD3D12Core.h"
#include "RenderConfig.h"
#include "MCRenderWindow.h"


using Microsoft::WRL::ComPtr;

namespace MC {

	class MCDXGI {
	public:
		/*
			MCDXGI is a singleton class. All copy constructors and assignment operators are deleted.
		*/
		MCDXGI(MCDXGI&)              = delete;
		MCDXGI(MCDXGI&&)             = delete;
		MCDXGI& operator= (MCDXGI&)  = delete;
		MCDXGI& operator= (MCDXGI&&) = delete;
		/*
			Simple destructor - MCDXGI owns no heap memory.
		*/
		~MCDXGI();

		/*
			Retrieve the singleton instance.
		*/
		static inline MCDXGI* Instance() {
			static MCDXGI instance;
			return &instance;
		}

	private:
		/*
			MCDXGI is a singleton. The constructor is private.
		*/
		MCDXGI();

	public:
		/*
			Initialize must be called on the MCDXGI instance prior to any other methods. Initialize attaches a target
			render window and a RENDER_CONFIG object to the MCDXGI instance.
		*/
		void Initialize(const RENDER_CONFIG* pConfig, std::shared_ptr<MCRenderWindow>& renderWindow);
		
		/*
			Return true if the instance has been initialized (Initialize has been called). Otherwise, return false.
		*/
		inline bool Initialized() { return _initialized; }
		
		/*
			Creates a new device based on the configuration and the render window supplied to the Initialize method.

			NOTE:
				CreateConfiguredOrDefault3DDevice must be called in order to create the internal ID3D12Device. Other
				methods will fail if this method has not been called.
				
				Further, CreateConfiguredOrDefault3DDeviceonly must be called once and only once.
		*/
		ID3D12Device *CreateConfiguredOrDefault3DDevice();

		/*
			Creates a swap chain for the given ID3D12CommandQueue.
		*/
		IDXGISwapChain3 *CreateConfiguredOrDefaltSwapchain(ID3D12CommandQueue *pCommandQueue);

		/*
			Access the ID3DDevice previously created by the call to CreateConfiguredOrDefault3DDevice.
		*/
		ID3D12Device *Get3DDevice();
		

		/*
			Internally calls present on the swap-chain.

			Note:
				After calling Present. The frame at the index previously returned from GetCurrentBackBufferIndex
				will now be drawn to the screen and GetCurrentBackBufferIndex will return a new index.
		*/
		HRESULT Present();

		

		/*
			Query the memory info the adapter associated with the current ID3D12Device.
		*/
		void QueryLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const;
		void QueryNonLocalMemoryInfo(DXGI_QUERY_VIDEO_MEMORY_INFO *pMemoryInfo) const;

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

		/*
			Get the dimensions of (one of) the framebuffer. The number of buffers actually being used by
			the swapchain is not relevant because they all must be the same size.
		*/
		void GetFrameBufferSize(int *pWidth, int *pHeight);

	private:
		IDXGIAdapter *CreateConfiguredOrDefaultAdapter();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter *pAdapter);
		IDXGIAdapter *GetAdapterByDeviceID(UINT deviceId);
		IDXGIAdapter *GetDefaultAdapter();
		void LogOutputDisplayModes(IDXGIOutput *pOutput, DXGI_FORMAT format);
		void InitDXGIFactory();
		void InitConfiguration(const RENDER_CONFIG *pConfig);
		void EnsureValidWindowConfig();

		void EnableDXDebugLayer();

		bool                            _initialized;
		const RENDER_CONFIG             _initialConfiguration;
		std::shared_ptr<MCRenderWindow> _pRenderWindow;

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
		ComPtr<IDXGIAdapter3>   _pAdapter;
	};

}
