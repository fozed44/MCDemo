#include "MCResourceManagerAnalyzer.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Console/MCConsoleCommand.h"

#include <sstream>
#include <iomanip>

namespace MC {

#pragma region ctor

	MCResourceManagerAnalyzer::MCResourceManagerAnalyzer(MCResourceManager *pManager)
		: _pManager{ pManager } {
		assert(_pManager);
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
	}

#pragma endregion

#pragma region process message overrides

	std::string MCResourceManagerAnalyzer::GetSummary() {
		return GenerateSummary();
	}

	std::string MCResourceManagerAnalyzer::GetDetail() {
		return GenerateDetail();
	}

#pragma endregion

#pragma region Analyze Resource Manager

	std::string MCResourceManagerAnalyzer::GenerateSummary() {
		auto linearBufferAnalyzer = _pManager->GetBufferAnalyzer();
		return linearBufferAnalyzer->Summary();
	}

	std::string MCResourceManagerAnalyzer::GenerateDetail() {
		auto linearBufferAnalyzer = reinterpret_cast<MCLinearBufferAnalyzer<MCManagedLinearBufferHandleManager::ManagedContextItem, MCRESOURCE_MANAGER_BUFFER_SIZE>*>(_pManager->GetBufferAnalyzer().get());
		return linearBufferAnalyzer->Detail(
			[](MC::MCResourceDescriptor element, MC::MCLinearBufferAddress address) {
				std::ostringstream o;

				o << std::left << std::setw(20) << element.dbgName;
				return o.str();
			}
		);
	}

#pragma endregion

}
