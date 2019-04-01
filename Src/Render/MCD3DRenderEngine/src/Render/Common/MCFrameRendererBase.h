#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"

#include <string>

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRendererBase {
	public: /* ctor / dtor / assignment*/
		MCFrameRendererBase(const std::string& name, unsigned int frameIndex);
		~MCFrameRendererBase();
		MCFrameRendererBase(MCFrameRendererBase&)              = delete;
		MCFrameRendererBase(MCFrameRendererBase&&)             = delete;
		MCFrameRendererBase& operator= (MCFrameRendererBase&)  = delete;
		MCFrameRendererBase& operator= (MCFrameRendererBase&&) = delete;

	public: /* Methods to return the name of the renderer. */

		const std::string& Name() const { return _name; }
		      std::string& Name()       { return _name; }

	protected:

		void AcquireRenderTargetAlias();

	protected: /* D3D12 Objects */

		ComPtr<ID3D12GraphicsCommandList> _pCommandList;
		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;

		ComPtr<ID3D12DescriptorHeap>      _CBVDescriptorHeap;

		ID3D12Resource*                   _pRenderTargetAlias; 

		DirectX::XMFLOAT4X4 _projectionMatrix;

		D3D12_VIEWPORT _viewPort;
		D3D12_RECT     _scissorRect;

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

		/* Associates this renderer with an index into the MCD3D._ppRenderTargets array. Assigned once
		   and only once during construction. */
		const unsigned int _frameIndex;
	};

}

