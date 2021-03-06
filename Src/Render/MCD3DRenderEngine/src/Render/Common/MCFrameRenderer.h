#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"
#include "../../Render/Common/MCRenderItem.h"
#include "../Common/MCRenderScene.h"
#include "../../Core/MCRenderObjects.h"

#include <string>

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRenderer {
	public: /* ctor / dtor / assignment*/
		MCFrameRenderer(const std::string& name, unsigned int frameIndex, MCRenderScene* pScene);
		virtual ~MCFrameRenderer();
		MCFrameRenderer(MCFrameRenderer&)              = delete;
		MCFrameRenderer(MCFrameRenderer&&)             = delete;
		MCFrameRenderer& operator= (MCFrameRenderer&)  = delete;
		MCFrameRenderer& operator= (MCFrameRenderer&&) = delete;

	public: /* Methods to return the name of the renderer. */

		const std::string& Name() const { return _name; }
		      std::string& Name()       { return _name; }

	public: /* RenderFrame */
		virtual void PrepareCommandLists(MCFrame *pVframe) = 0;
		virtual unsigned __int64 ExecuteCommandLists()     = 0;

		/* Use the value of _frameIndex (set during construction) to acquire the render target from MCDXGI */
		void AcquireRenderTargetInfo();

		/* for debugging... delete later.*/
		unsigned int FrameIndex() { return _frameIndex; }

		virtual void OnResizing() = 0;

	protected: /* D3D12 Objects */
		unsigned int					  _frameIndex;
		ID3D12Resource*     			  _pRenderTarget;
		ID3D12Resource*                   _pDepthStencil;

		D3D12_CPU_DESCRIPTOR_HANDLE		  _hCPURenderTarget;
		D3D12_GPU_DESCRIPTOR_HANDLE	      _hGPURenderTarget;

		D3D12_CPU_DESCRIPTOR_HANDLE		  _hCPUDepthStencil;
		D3D12_GPU_DESCRIPTOR_HANDLE		  _hGPUDepthStencil;

		ComPtr<ID3D12GraphicsCommandList> _pCommandList;
		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;

		ComPtr<ID3D12DescriptorHeap>      _CBVDescriptorHeap;

		DirectX::XMFLOAT4X4               _projectionMatrix;

		// TODO:
		// delete me
		//float                             _aspectRatio;
		float                             _fov;
		float                             _farPlane;

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

	protected:
		/* Pointer to the render scene. */
		MCRenderScene* _pScene;
	};

}

