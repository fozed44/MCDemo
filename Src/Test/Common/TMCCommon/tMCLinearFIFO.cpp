#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>

// Ensure that _DEBUG_MCLINEAR_FIFO__ is defined event if it is missing from GlobalSwitches.h
#define __DEBUG_MCLINEAR_FIFO__

#include "../../../Common/MCCommon/src/Data/MCLinearFIFO.h"
#include "../../../../Src/Common/MCRouter/src/Core/MCMessage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon
{
	TEST_CLASS(TestMCLinearFIFO)
	{
	public:

		/* Attempting to call push after buffer will throw in debug mode. */
		TEST_METHOD(PushPastBuffer_Fail) {
			bool exceptionThrown = false;

			MCLinearFIFO<MC::MC_MESSAGE128, 2> test;
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			try {
				test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			}
			catch (...){
				exceptionThrown = true;
			}
			Assert::IsTrue(exceptionThrown);
		}

		/* Attempting to call peek before calling beginRead should throw in debug mode. */
		TEST_METHOD(PeekBeforeBeginRead_Fail) {
			bool exceptionThrown = false;

			MCLinearFIFO<MC::MC_MESSAGE128, 2> test;
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			try {
				auto result = test.peek();
			}
			catch (...) {
				exceptionThrown = true;
			}
			Assert::IsTrue(exceptionThrown);
		}

		/*	Attempting to call pop before calling beginRead should throw in debug mode.
		*/
		TEST_METHOD(PopBeforeBeginRead_Fail) {
			bool exceptionThrown = false;

			MCLinearFIFO<MC::MC_MESSAGE128, 2> test;
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			try {
				MC_MESSAGE128 result;
				test.pop(&result);
			}
			catch (...) {
				exceptionThrown = true;
			}
			Assert::IsTrue(exceptionThrown);
		}

		/* Attempting to call pop more times than push (attempting to read beyond valid data should throw
		   in debug mode. */
		TEST_METHOD(PopPastPush_Fail) {
			bool exceptionThrown = false;

			MCLinearFIFO<MC::MC_MESSAGE128, 3> test;
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			test.push({ MC_MESSAGE_INVALID, MC_MESSAGE_VISIBILITY_NONE, MC_MESSAGE_FLAGS_NONE, 0, 0 });
			test.allow_write(false);
			MC_MESSAGE128 result;
			bool more;

			more = test.pop(&result);

			// We have only read the first element, since this is not the last element, more should be true.
			Assert::IsTrue(more);

			more = test.pop(&result);

			// This is still not the last element, more should still not be false.
			Assert::IsTrue(more);

			// There should be no more elements left.
			more = test.pop(&result);

			// So more should now be false.
			Assert::IsFalse(exceptionThrown);
		}
	};
}