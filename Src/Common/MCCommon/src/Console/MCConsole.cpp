#include "MCConsole.h"
#include "../../../MCCommon/src/Data/MCThreads.h"

#include "windows.h"
#include <vector>
namespace MC {

#pragma region MCConsoleCommandParser

	MCConsoleCommandParser::MCConsoleCommandParser(tAllocator allocator) {
		_fpAllocator = allocator;
	}

	MCConsoleCommandParser::~MCConsoleCommandParser() {}

	MC_RESULT MCConsoleCommandParser::Parse(char *pBuffer, size_t bufferSize, MC_CONSOLE_COMMAND* pCommand) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
		std::vector<std::string> commandStrings;
		auto result = ParseStrings(pBuffer, bufferSize, &commandStrings);

		if (result != MC_RESULT_OK)
			return result;

		pCommand->Command = TranslateCommand(commandStrings[0]);

		if (pCommand->Command == MC_CONSOLE_COMMAND_CMD_INVALID)
			return MC_RESULT_FAIL_INVALID_DATA;

		pCommand->ParameterCount = commandStrings.size() - 1;

		if (pCommand->ParameterCount == 0) {
			pCommand->pParameterData = nullptr;
			return MC_RESULT_OK;
		}

		commandStrings.erase(commandStrings.begin());

		pCommand->pParameterData =  AllocateParameterData(commandStrings);

		return MC_RESULT_OK;
	}

	MC_RESULT MCConsoleCommandParser::ParseStrings(char *pBuffer, size_t bufferSize, std::vector<std::string>* pResult) {
		char *elementStart{ pBuffer }, *elementEnd{ pBuffer };
		for (unsigned int x = 0; x < bufferSize; x++) {
			elementEnd++;

			if ('\0' == *elementEnd) {
				pResult->push_back(std::string(elementStart, elementEnd) + std::string("\n"));
				return MC_RESULT_OK;
			}

			if (' ' == *elementEnd) {
				pResult->push_back(std::string(elementStart, elementEnd) + std::string("\n"));
				elementStart = elementEnd;
			}
		}
		return MC_RESULT_FAIL_INVALID_DATA;
	}

	char** MCConsoleCommandParser::AllocateParameterData(const std::vector<std::string>& commandArgs) {
		std::vector<char*> ptrs;
		char** ptrArray = reinterpret_cast<char**>(_fpAllocator(sizeof(char*) * commandArgs.size()));
		for (int x = 0; x < commandArgs.size(); ++x) {
			char *pCommand = _fpAllocator(commandArgs[x].size() + 1);
			strcpy(pCommand, commandArgs[x].c_str());
			ptrArray[x] = pCommand;
		}
		return ptrArray;
	}

	MC_CONSOLE_COMMAND_CMD MCConsoleCommandParser::TranslateCommand(const std::string& command) {
		auto commandMapElement = _commandMap.find(command);
		if (commandMapElement == _commandMap.end())
			return MC_CONSOLE_COMMAND_CMD_INVALID;
		return commandMapElement->second;
	}

#pragma endregion

#pragma region ctor

	MCConsole::MCConsole() {}

	MCConsole::~MCConsole() {}

#pragma endregion

#pragma region Process Messages

	void MCConsole::ProcessMessage32(MC_MESSAGE32 message) {
		switch (message.Message) {
		case MC_MESSAGE_KEY_DOWN_32:
			NewKey(message.Param);
		}
	}

#pragma endregion

	void MCConsole::NewKey(unsigned char vkKey) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		if (_pNext == _pEnd)
			return;
		*_pNext = toascii(vkKey);
		_pNext++;
		
		if (vkKey == VK_RETURN) {
			auto command = _commandParser.Parse(_pKeyBuffer, CONSOLE_KEY_BUFFER_SIZE);
		}
	}


}