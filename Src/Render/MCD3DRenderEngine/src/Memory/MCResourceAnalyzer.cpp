#pragma once

#include "MCResourceAnalyzer.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Console/MCConsoleCommand.h"

namespace MC {

#pragma region ctor

	MCResourceAnalyzer::MCResourceAnalyzer(MCResourceManager *pManager, MCRouter* pRouter) 
		: _pManager{ pManager },
		  _pRouter { pRouter } {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		pRouter->RegisterDispatchTarget(this, MC_MESSAGE_VISIBILITY_ANALYZE);
	}

	MCResourceAnalyzer::~MCResourceAnalyzer() {
		if (_pRouter)
			_pRouter->UnregisterDispatchTarget(this);
	}

#pragma endregion

#pragma region process message overrides

	void MCResourceAnalyzer::OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) {
		if (message.Message == MC_MESSAGE_CONSOLE_COMMAND_128) {
			if(message.LOParam32 == MC_CONSOLE_COMMAND_CMD_ANALYZE_RESOURCE_MAN)
				AnalyzeResourceManager();
		}
	}

#pragma endregion

#pragma region Analyze Resource Manager

	void MCResourceAnalyzer::AnalyzeResourceManager() {
		// This method MUST be executed on the main thread because it accesses the router
		// without using a router lock.
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		auto analysisResult = GenerateAnalysis();

		MC_MESSAGE128 msg{};
		msg.Message    = MC_MESSAGE_CONSOLE_OUTPUT_128;
		msg.Visibility = MC_MESSAGE_VISIBILITY_CONSOLE;
		auto ptr = _pRouter->PushTo(msg, static_cast<unsigned int>(analysisResult.size() * sizeof(char)));
		strcpy_s(ptr, analysisResult.size() + 1, analysisResult.c_str());
	}

	std::string MCResourceAnalyzer::GenerateAnalysis() {
		auto linearBufferAnalyzer = _pManager->GetBufferAnalyzer();
		return linearBufferAnalyzer->Analyze();
	}

#pragma endregion

}
