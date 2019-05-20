#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"
#include "../../../../Common/MCCommon/src/Analyzers/MCLinearBufferAnalyzer.h"
#include "../Memory/MCResourceManager.h"

#include <memory>

namespace MC {

	class MCResourceManagerAnalyzer {
	public:
		MCResourceManagerAnalyzer(MCResourceManager* pManager);
		~MCResourceManagerAnalyzer() {}
		MCResourceManagerAnalyzer(MCResourceManagerAnalyzer&)              = delete;
		MCResourceManagerAnalyzer(MCResourceManagerAnalyzer&&)             = delete;
		MCResourceManagerAnalyzer& operator= (MCResourceManagerAnalyzer&)  = delete;
		MCResourceManagerAnalyzer& operator= (MCResourceManagerAnalyzer&&) = delete;
	public:
		std::string GetAnalysis();
	private:
		std::string GenerateAnalysis();
	private:
		MCResourceManager*           _pManager;
		MCRouter*                    _pRouter;
		std::unique_ptr<MCIAnalyzer> _pAnalyzer;
	};

}