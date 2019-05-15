#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"
#include "../../../../Common/MCCommon/src/Analyzers/MCLinearBufferAnalyzer.h"
#include "../Memory/MCResourceManager.h"

#include <memory>

namespace MC {

	class MCResourceAnalysisDispatchTarget : public MCMessageDispatchTarget {
	public:
		MCResourceAnalysisDispatchTarget(MCResourceManager* pManager, MCMessageDispatchTarget* pParent, MCRouter* pRouter);
		~MCResourceAnalysisDispatchTarget() {}
		MCResourceAnalysisDispatchTarget(MCResourceAnalysisDispatchTarget&)              = delete;
		MCResourceAnalysisDispatchTarget(MCResourceAnalysisDispatchTarget&&)             = delete;
		MCResourceAnalysisDispatchTarget& operator= (MCResourceAnalysisDispatchTarget&)  = delete;
		MCResourceAnalysisDispatchTarget& operator= (MCResourceAnalysisDispatchTarget&&) = delete;

	protected:
		virtual void OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;

	private:
		void AnalyzeResourceManager();
		std::string GenerateAnalysis();
	private:
		MCResourceManager*           _pManager;
		MCRouter*                    _pRouter;
		std::unique_ptr<MCIAnalyzer> _pAnalyzer;
	};

}