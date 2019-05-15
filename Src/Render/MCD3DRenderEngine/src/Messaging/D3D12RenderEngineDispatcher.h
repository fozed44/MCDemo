#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"
#include "assert.h"

namespace MC {

	/* D3D12RenderEngineDispatcher

		This dispatcher is the top-level dispatcher for the rendering engine. The constructor will
		attach the dispatcher to the pRouter parameter. Within the context of the render engine,
		this dispatcher will be created by the constructor of MCD3D12RenderEngine, which will also
		add a reference to the instance to MCREGlobals.

		All sub-dispatchers within the render framework will attach themselves to this dispatcher,
		allowing this dispatcher to be the messaging gateway to the rendering engine.

		This dispatcher will not itself be handling any messages; it will only be passing messages to
		sub-dispatchers.
	*/
	class D3D12RenderEngineDispatcher : public MCMessageDispatchTarget {
	public:
		D3D12RenderEngineDispatcher(MC::MCRouter *pRouter) { assert(pRouter); pRouter->RegisterDispatchTarget(this, MC_MESSAGE_VISIBILITY_RENDER); }
		virtual ~D3D12RenderEngineDispatcher() {};
		D3D12RenderEngineDispatcher(D3D12RenderEngineDispatcher&)              = delete;
		D3D12RenderEngineDispatcher(D3D12RenderEngineDispatcher&&)             = delete;
		D3D12RenderEngineDispatcher& operator= (D3D12RenderEngineDispatcher&)  = delete;
		D3D12RenderEngineDispatcher& operator= (D3D12RenderEngineDispatcher&&) = delete;
	};

}
