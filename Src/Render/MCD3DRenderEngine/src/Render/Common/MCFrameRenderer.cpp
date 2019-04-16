#include "MCFrameRenderer.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../Core/MCREGlobals.h"

namespace MC {

#pragma region MCFrameRenderer static variables

	UINT MCFrameRenderer::_CBVDescriptorIncrementSize = 0;

#pragma endregion

	MCFrameRenderer::MCFrameRenderer(const std::string& name, unsigned int frameIndex) 
		: _name{ name },
		  _frameIndex{ frameIndex } {

		// A Frame renderer should only be constructed on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS::MAIN);

		InitializeBase();
		AcquireRenderTargetInfo();
	}


	MCFrameRenderer::~MCFrameRenderer() { 
		// A Frame renderer should only be destructed on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS::MAIN);		
	}

#pragma region Initialization

	void MCFrameRenderer::InitializeBase() {
		INIT_TRACE("Begin initialization of renderer {0}.", _name);

		InitializeCommandObjects();
		InitializeViewPort();

		INIT_TRACE("End initialization of renderer {0}.", _name);
	}

	void MCFrameRenderer::InitializeCommandObjects() {
		INIT_TRACE("Initializing command allocator for renderer {0}.", _name);
		MCThrowIfFailed(
			MCREGlobals::pMCDXGI->Get3DDevice()->
			CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				__uuidof(_pCommandAllocator),
				&_pCommandAllocator
			)
		);
		INIT_TRACE("Initialization complete.");

		INIT_TRACE("Initializing command list for renderer {0}.", _name);
		MCThrowIfFailed(
			MCREGlobals::pMCDXGI->Get3DDevice()->
			CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				_pCommandAllocator.Get(),
				nullptr,
				__uuidof(_pCommandList),
				&_pCommandList
			)
		);

		_pCommandList->Close();

		INIT_TRACE("Initialization complete.");
	}

	void MCFrameRenderer::InitializeViewPort() {
		INIT_TRACE("Begin init view port.");

		auto width  = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_WIDTH;
		auto height = MCREGlobals::InitialConfiguration.DISPLAY_OUTPUT_HEIGHT;

		_viewPort = {};
		_viewPort.TopLeftX = 0.0f;
		_viewPort.TopLeftY = 0.0f;
		_viewPort.Width    = static_cast<float>(width);
		_viewPort.Height   = static_cast<float>(height);
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		_scissorRect.top    = 0;
		_scissorRect.left   = 0;
		_scissorRect.right  = width;
		_scissorRect.bottom = height;

		INIT_TRACE("End view port.");
	}

#pragma endregion

#pragma region RenderTarget acquizition

	void MCFrameRenderer::AcquireRenderTargetInfo() {
		MCREGlobals::pMCDXGI->GetFrameBuffer(_frameIndex, __uuidof(_pRenderTarget), (void**)&_pRenderTarget);
		_pDepthStencil = MCREGlobals::pMCD3D->GetDepthStencil();

		_hCPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetCPUHandle(_frameIndex);
		_hGPURenderTarget = MCREGlobals::pMCD3D->GetRenderTargetGPUHandle(_frameIndex);

		_hCPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilCPUHandle();
		_hGPUDepthStencil = MCREGlobals::pMCD3D->GetDepthStencilGPUHandle();
	}

#pragma endregion

}
