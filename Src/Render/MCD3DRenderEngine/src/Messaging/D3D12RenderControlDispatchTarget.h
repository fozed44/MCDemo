#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"

namespace MC {

	/* D3D12RenderControlDispatchTarget
		The RenderControlDispatchTarget listens for MC_MESSAGE_VISIBILITY_SYSTEM messages. This dispatch
		target listens for commands that are relevant to control of the render engine. The commands are
		usually handled by pushing commands back to the router that will be picked up by the render engine
		dispatcher.
	*/
	class D3D12RenderControlDispatchTarget : public MCMessageDispatchTarget {
	public:
		D3D12RenderControlDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter);
		~D3D12RenderControlDispatchTarget() {}
		D3D12RenderControlDispatchTarget(D3D12RenderControlDispatchTarget&)              = delete;
		D3D12RenderControlDispatchTarget(D3D12RenderControlDispatchTarget&&)             = delete;
		D3D12RenderControlDispatchTarget& operator= (D3D12RenderControlDispatchTarget&)  = delete;
		D3D12RenderControlDispatchTarget& operator= (D3D12RenderControlDispatchTarget&&) = delete;

	protected:
		virtual void OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;

	private:
		void HandleRendererCommand(unsigned char parameterCount, char **pParameterData);
	private:
		MCRouter* _pRouter;
	};

}
