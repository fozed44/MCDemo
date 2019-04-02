#include "MCFrameRendererBase.h"

#include "../../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../Core/MCREGlobals.h";

namespace MC {

#pragma region MCFrameRendererBase static variables

	UINT MCFrameRendererBase::_CBVDescriptorIncrementSize = 0;

#pragma endregion

	MCFrameRendererBase::MCFrameRendererBase(const std::string& name, unsigned int frameIndex) 
		: _name{ name },
		  _frameIndex{ frameIndex } {

		// A Frame renderer should only be constructed on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);

		InitializeBase();
	}


	MCFrameRendererBase::~MCFrameRendererBase() { 
		// A Frame renderer should only be destructed on the main thread.
		MCThreads::Assert(MC_THREAD_CLASS_MAIN);
	}

#pragma region Initialization

	void MCFrameRendererBase::InitializeBase() {
		INIT_TRACE("Begin initialization of renderer {0}.", _name);

		InitializeCommandObjects();
		InitializeViewPort();

		INIT_TRACE("End initialization of renderer {0}.", _name);
	}

	void MCFrameRendererBase::InitializeCommandObjects() {
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
		INIT_TRACE("Initialization complete.");
	}

	void MCFrameRendererBase::InitializeViewPort() {
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

}
