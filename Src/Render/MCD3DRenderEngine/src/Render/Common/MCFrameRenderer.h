#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"

#include <string>

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCFrameRendererTargetInfo {
		unsigned int FrameIndex;
		ID3D12Resource *pRenderTarget;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPURenderTarget;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPURenderTarget;

		ID3D12Resource *pDepthStencil;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUDepthStencil;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUDepthStencil;
	};

	class MCFrameRenderer {
	public: /* ctor / dtor / assignment*/
		MCFrameRenderer(const std::string& name);
		virtual ~MCFrameRenderer();
		MCFrameRenderer(MCFrameRenderer&)              = delete;
		MCFrameRenderer(MCFrameRenderer&&)             = delete;
		MCFrameRenderer& operator= (MCFrameRenderer&)  = delete;
		MCFrameRenderer& operator= (MCFrameRenderer&&) = delete;

	public: /* Methods to return the name of the renderer. */

		const std::string& Name() const { return _name; }
		      std::string& Name()       { return _name; }

	public: /* RenderFrame */
		virtual unsigned __int64 RenderFrame(MCFrame *pVframe, const MCFrameRendererTargetInfo& frameTarget) = 0;

	protected: /* D3D12 Objects */

		ComPtr<ID3D12GraphicsCommandList> _pCommandList;
		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;

		ComPtr<ID3D12DescriptorHeap>      _CBVDescriptorHeap;

		DirectX::XMFLOAT4X4               _projectionMatrix;

		D3D12_VIEWPORT                    _viewPort;
		D3D12_RECT                        _scissorRect;

	protected: /* Descriptor handle increment sizes. */

		static UINT _CBVDescriptorIncrementSize;

	private: /* INITIALIZATION METHODS */

		/* Directly called by the constructor to initialize the renderer. */
		void InitializeBase();

		/* Initialize the command list and allocator */
		void InitializeCommandObjects();
		
		/* Initialize the view port. */
		void InitializeViewPort();

	private:

		/* Identifies this renderer. This is the value passed to the name parameter of the constructor. */
		std::string _name;
	};

}

