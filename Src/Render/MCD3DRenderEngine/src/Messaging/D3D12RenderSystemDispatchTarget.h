#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"

namespace MC {

	/* D3D12RenderSystemDispatchTarget
		The RenderSystemDispatchTarget listens for MC_MESSAGE_VISIBILITY_SYSTEM messages. This dispatch
		target listens for render messages (messages must be render messages because this dispatch target
		should be attached to the RenderEngineDispatchTarget. The systems messages are usually handled by
		pushing messages back to the router that will be picked up by the render engine and handles by 
		some other sub-dispatch target.

		Probably most of the system messages will be comprised of command messages. So this does mean that
		this dispatch target will handle console command messages, convert them to another message that 
		will be passed to the router, which will put the message into the back buffer, which on the next
		frame will be dispatched to another dispatch target that will probably reside on this side of the
		render engine dispatcher... obviously this is a little inefficient.
	*/
	class D3D12RenderSystemDispatchTarget : public MCMessageDispatchTarget {
	public:
		D3D12RenderSystemDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter);
		~D3D12RenderSystemDispatchTarget() {}
		D3D12RenderSystemDispatchTarget(D3D12RenderSystemDispatchTarget&)              = delete;
		D3D12RenderSystemDispatchTarget(D3D12RenderSystemDispatchTarget&&)             = delete;
		D3D12RenderSystemDispatchTarget& operator= (D3D12RenderSystemDispatchTarget&)  = delete;
		D3D12RenderSystemDispatchTarget& operator= (D3D12RenderSystemDispatchTarget&&) = delete;

	protected:
		virtual void OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;

	private:
		void HandleRenderCommand(unsigned char parameterCount, char **pParameterData);
		void HandleReportCommand(unsigned char parameterCount, char **pParameterData);
	private:
		MCRouter* _pRouter;
	};

}
