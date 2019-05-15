#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"
#include "../../../../Common/MCCommon/src/Analyzers/MCLinearBufferAnalyzer.h"
#include "MCResourceManager.h"

#include <memory>

namespace MC {

	class MCResourceAnalysisDispatcher : public MCMessageDispatchTarget {
	public:
		MCResourceAnalysisDispatcher(MCResourceManager* pManager, MCMessageDispatchTarget* pParent, MCRouter* pRouter);
		~MCResourceAnalysisDispatcher() {}
		MCResourceAnalysisDispatcher(MCResourceAnalysisDispatcher&)              = delete;
		MCResourceAnalysisDispatcher(MCResourceAnalysisDispatcher&&)             = delete;
		MCResourceAnalysisDispatcher& operator= (MCResourceAnalysisDispatcher&)  = delete;
		MCResourceAnalysisDispatcher& operator= (MCResourceAnalysisDispatcher&&) = delete;

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