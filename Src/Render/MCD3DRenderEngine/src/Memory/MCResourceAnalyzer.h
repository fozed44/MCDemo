#pragma once

#include "../../../../Common/MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../../../../Common/MCRouter/src/Core/MCRouter.h"
#include "../../../../Common/MCCommon/src/Analyzers/MCLinearBufferAnalyzer.h"
#include "MCResourceManager.h"

#include <memory>

namespace MC {

	class MCResourceAnalyzer : public MCMessageDispatchTarget {
	public:
		MCResourceAnalyzer(MCResourceManager* pManager, MCRouter* pRouter);
		~MCResourceAnalyzer();
		MCResourceAnalyzer(MCResourceAnalyzer&)              = delete;
		MCResourceAnalyzer(MCResourceAnalyzer&&)             = delete;
		MCResourceAnalyzer& operator= (MCResourceAnalyzer&)  = delete;
		MCResourceAnalyzer& operator= (MCResourceAnalyzer&&) = delete;

	protected:
		virtual void ProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;

	private:
		void AnalyzeResourceManager();
		std::string GenerateAnalysis();
		std::string ActiveObjects();
	private:
		MCResourceManager*           _pManager;
		MCRouter*                    _pRouter;
		std::unique_ptr<MCIAnalyzer> _pAnalyzer;
	};

}