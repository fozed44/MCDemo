#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>

// Ensure that _DEBUG_MCLINEAR_FIFO__ is defined event if it is missing from GlobalSwitches.h
#define __DEBUG_MCLINEAR_FIFO__

// Ensure that __DEBUG_ROUTING__ is defined even if it is missing from GlobalSwitches.h
#define __DEBUG_ROUTING__

#include "../../../Common/MCRouter/src/Core/MCMessageQueue.h"
#include "../../../../Src/Common/MCRouter/src/Core/MCMessage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon {

	struct Person {
		Person(int t1, int t2) { testProp1 = t1, testProp2 = t2; }
		~Person() {
			testProp2 = testProp1;
		}
		int testProp1;
		int testProp2;
	};

	TEST_CLASS(TEST_MCMessageQueue)
	{
		TEST_CLASS_INITIALIZE(UseNullLoggers) {
			MC::MCLogHelper::SetNullLoggers();
		}

		TEST_METHOD(Instantiate) {
			MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, 5, 200> queue("test queue");

			Assert::AreEqual(200, (int)queue.front_buffer_size());
			Assert::AreEqual(200, (int)queue.front_buffer_free());
			Assert::AreEqual(200, (int)queue.back_buffer_size());
			Assert::AreEqual(200, (int)queue.back_buffer_free());
		}

		TEST_METHOD(StoreData) {
			MCThreads::RegisterCurrentThread("Unit test", MC_THREAD_CLASS_MAIN);

			MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, 5, 200> queue("test queue");

			MC_MESSAGE128 msg;

			msg.Message = MC_MESSAGE_FRAME_READY_128;

			Person* ptr = queue.push_to<Person>(msg);
			ptr->testProp1 = 1;
			ptr->testProp2 = 2;

			msg = {}; ptr = nullptr;

			queue.swap();

			bool valid = queue.pop(&msg);
			ptr = reinterpret_cast<Person*>(msg.pAddress);

			Assert::AreEqual(1, ptr->testProp1);
			Assert::AreEqual(2, ptr->testProp2);			
		}

		TEST_METHOD(MemoryReleasedAfterSwap) {
			MCThreads::RegisterCurrentThread("Unit test", MC_THREAD_CLASS_MAIN);

			MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, 5, 200> queue("test queue");

			MC_MESSAGE128 msg;

			msg.Message = MC_MESSAGE_FRAME_READY_128;

			Person* ptr = queue.push_to<Person>(msg);

			int t = sizeof(Person);
			int x = (sizeof(Person) - 1) / 4 * 4 + 4;
			// The front buffer should have a size of 192: 200 bytes minus the size of Person.
			Assert::AreEqual(192, (int)queue.front_buffer_free());
			Assert::AreEqual(200, (int)queue.back_buffer_free());

			queue.swap();

			// After the swap, the back buffer should still be at 192, the memory shouldn't be released until
			// the back buffer (currently in read mode) gets swapped back to write mode (set to the front buffer).

			Assert::AreEqual(200, (int)queue.front_buffer_free());
			Assert::AreEqual(192, (int)queue.back_buffer_free());

			queue.swap();

			// Now the memory should be freed.
			Assert::AreEqual(200, (int)queue.front_buffer_free());
			Assert::AreEqual(200, (int)queue.back_buffer_free());
		}

		TEST_METHOD(WritePastBufferEndThrowsA) {
			MCThreads::RegisterCurrentThread("Unit test", MC_THREAD_CLASS_MAIN);

			MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, 5, 200> queue("test queue");

			MC_MESSAGE128 msg;

			msg.Message = MC_MESSAGE_FRAME_READY_128;

			auto exceptionThrown = false;

			try {
				char* ptr = queue.push_to(msg, 201);
			} catch (...) {
				exceptionThrown = true;
			}

			Assert::IsTrue(exceptionThrown);
		}

		TEST_METHOD(WritePastBufferEndThrowsB) {
			MCThreads::RegisterCurrentThread("Unit test", MC_THREAD_CLASS_MAIN);

			MCMessageQueueWithMemoryBuffer<MC_MESSAGE128, 202, 200> queue("test queue");

			MC_MESSAGE128 msg;

			msg.Message = MC_MESSAGE_FRAME_READY_128;


			bool exceptionThrown = false;

			try {
				for(int x = 0; x < 199; x++)
					char* ptr = queue.push_to(msg,1);
			}
			catch (...) {
				exceptionThrown = true;
			}

			Assert::IsTrue(exceptionThrown);
		}

		TEST_METHOD(WritePastQueueEndThrows) {

		}

		TEST_METHOD(ReadPastBufferEndThrows) {

		}
		TEST_METHOD(ReadPastQueueEndThrows) {

		}

	};

}