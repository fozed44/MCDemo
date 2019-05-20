#include "stdafx.h"

#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCCommon/src/Console/MCConsoleCommandParser.h"
#include "../../../Common/MCCommon/src/Data/MCThreads.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon {

	char *test_allocator(size_t size) {
		return reinterpret_cast<char*>(malloc(size));
	}

	TEST_CLASS(TestMCConsoleCommandParser) {		

		TEST_CLASS_INITIALIZE(UseNullLoggers) {
			MCLogHelper::SetNullLoggers();
			MCThreads::RegisterCurrentThread("unit test", MC_THREAD_CLASS::MAIN);
		}

		TEST_METHOD(TestSingleCommand) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "WIREFRAME";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::OK, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::WIRE_FRAME, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(TestCommandWithSingleArg) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "WIREFRAME testArg";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::OK, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::WIRE_FRAME, (int)cmd.Command);
			Assert::AreEqual(1, (int)cmd.ParameterCount);
			Assert::AreEqual("testArg", cmd.pParameterData[0]);

			delete cmd.pParameterData[0];
			delete (char*)cmd.pParameterData;
		}

		TEST_METHOD(TestCommandWithTwoArgs) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "WIREFRAME testArg1 testArg2";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::OK, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::WIRE_FRAME, (int)cmd.Command);
			Assert::AreEqual(2, (int)cmd.ParameterCount);
			Assert::AreEqual("testArg1", cmd.pParameterData[0]);
			Assert::AreEqual("testArg2", cmd.pParameterData[1]);

			delete cmd.pParameterData[0];
			delete cmd.pParameterData[1];
			delete (char*)cmd.pParameterData;
		}

		TEST_METHOD(TestInvalidCommand) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "invalid-command testArg";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::FAIL_INVALID_DATA, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::INVALID, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(EmptyIsInvalid) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::FAIL_INVALID_DATA, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::INVALID, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(JustSpacesIsInvalid) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "  ";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::FAIL_INVALID_DATA, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::INVALID, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(TestInvalidCommandSingleArgumentExtraSpace) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "  invalid-command testArg  ";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::FAIL_INVALID_DATA, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::INVALID, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(TestInvalidCommandExtraSpaces) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "  invalid-command testArg  ";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::FAIL_INVALID_DATA, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::INVALID, (int)cmd.Command);
			Assert::AreEqual(0, (int)cmd.ParameterCount);
			Assert::AreEqual(nullptr, (char*)cmd.pParameterData);
		}

		TEST_METHOD(Test2ArgumentsExtraSpaces) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "  wire-frame testArg1 testArg2  ";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::OK, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::WIRE_FRAME, (int)cmd.Command);
			Assert::AreEqual(2, (int)cmd.ParameterCount);
			Assert::AreEqual("testArg1", cmd.pParameterData[0]);
			Assert::AreEqual("testArg2", cmd.pParameterData[1]);

			delete cmd.pParameterData[0];
			delete cmd.pParameterData[1];
			delete (char*)cmd.pParameterData;
		}

		TEST_METHOD(Test2ArgumentsExtraSpacesBetweenArguments) {
			MCConsoleCommandParser parser(test_allocator);

			MC_CONSOLE_COMMAND cmd;

			char *testCommand = "  wire-frame testArg1   testArg2  ";

			auto mcResult = parser.Parse(testCommand, strlen(testCommand), &cmd);

			Assert::AreEqual((int)MC_RESULT::OK, (int)mcResult);
			Assert::AreEqual((int)MC_CONSOLE_COMMAND_CMD::WIRE_FRAME, (int)cmd.Command);
			Assert::AreEqual(2, (int)cmd.ParameterCount);
			Assert::AreEqual("testArg1", cmd.pParameterData[0]);
			Assert::AreEqual("testArg2", cmd.pParameterData[1]);

			delete cmd.pParameterData[0];
			delete cmd.pParameterData[1];
			delete (char*)cmd.pParameterData;
		}
	};

}