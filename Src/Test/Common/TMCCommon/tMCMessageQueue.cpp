#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>

// Ensure that _DEBUG_MCLINEAR_FIFO__ is defined event if it is missing from GlobalSwitches.h
#define __DEBUG_MCLINEAR_FIFO__

// Ensure that __DEBUG_MCMESSAGE_QUEUE__ is defined even if it is missing from GlobalSwitches.h
#define __DEBUG_MCMESSAGE_QUEUE__

#include "../../../Common/MCCommon/src/Data/MCMessageQueue.h"
#include "../../../../Src/Common/MCRouter/src/Core/MCMessage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon {

	TEST_CLASS(TEST_MCMessageQueue)
	{

		TEST_METHOD(Instantiate) {
			MCMessageQueue<
				MC_MESSAGE128,
				5,
				200,
				[](const MC_MESSAGE128* t) { return t->Address },
				[](MC_MESSAGE128* t, unsigned short p) { t->LOParam16LO = p; }>
		}

	}

}