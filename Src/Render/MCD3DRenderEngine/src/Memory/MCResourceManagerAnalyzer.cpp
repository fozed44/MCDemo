#include "MCResourceManagerAnalyzer.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Console/MCConsoleCommand.h"

namespace MC {

#pragma region ctor

	MCResourceManagerAnalyzer::MCResourceManagerAnalyzer(MCResourceManager *pManager)
		: _pManager{ pManager } {
		assert(_pManager);
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
	}

#pragma endregion

#pragma region process message overrides

	std::string MCResourceManagerAnalyzer::GetAnalysis() {
		return GenerateAnalysis();
	}

#pragma endregion

#pragma region Analyze Resource Manager

	std::string MCResourceManagerAnalyzer::GenerateAnalysis() {
		auto linearBufferAnalyzer = _pManager->GetBufferAnalyzer();
		return linearBufferAnalyzer->Analyze();
	}

#pragma endregion

}
