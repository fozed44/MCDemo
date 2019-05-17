#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"

namespace MC {

	/* D3D12RenderEngineDispatchTarget
		The RenderEngineDispatchTarget Directly handles MC_MESSAGE_VISIBILITY_RENDERER messages.
	*/
	class D3D12RenderEngineDispatchTarget : public MCMessageDispatchTarget {
	public:
		D3D12RenderEngineDispatchTarget(MCMessageDispatchTarget* pParent, MCRouter* pRouter);
		~D3D12RenderEngineDispatchTarget() {}
		D3D12RenderEngineDispatchTarget(D3D12RenderEngineDispatchTarget&) = delete;
		D3D12RenderEngineDispatchTarget(D3D12RenderEngineDispatchTarget&&) = delete;
		D3D12RenderEngineDispatchTarget& operator= (D3D12RenderEngineDispatchTarget&) = delete;
		D3D12RenderEngineDispatchTarget& operator= (D3D12RenderEngineDispatchTarget&&) = delete;

	protected:
		virtual void OnProcessMessage32(MC_MESSAGE32 message) override;

	private:
		void SetPauseState(bool pauseState);
		void HandleRendererCommand(unsigned char parameterCount, char **pParameterData);
	private:
		MCRouter* _pRouter;
	};

}
