#include "MCConsoleCommandParser.h"
#include "../Data/MCThreads.h"

namespace MC {

	MCConsoleCommandParser::MCConsoleCommandParser(tAllocator allocator) {
		_fpAllocator = allocator;
		_commandMap = {
			{ "report-router-memory", MC_CONSOLE_COMMAND_CMD_REPORT_ROUTER_MEMORY },
			{ "wire-frame",           MC_CONSOLE_COMMAND_CMD_WIRE_FRAME },
			{ "analyze-resource-man", MC_CONSOLE_COMMAND_CMD_ANALYZE_RESOURCE_MAN },
			{ "renderer",             MC_CONSOLE_COMMAND_CMD_RENDERER },
			{ "analyze",              MC_CONSOLE_COMMAND_CMD_ANALYZE }
		};
	}

	MCConsoleCommandParser::~MCConsoleCommandParser() {}

	MC_RESULT MCConsoleCommandParser::Parse(char *pBuffer, size_t bufferSize, MC_CONSOLE_COMMAND* pCommand) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);
		std::vector<std::string> commandStrings;
		auto result = ParseStrings(pBuffer, bufferSize, &commandStrings);

		if (result != MC_RESULT::OK) {
			pCommand->Command = MC_CONSOLE_COMMAND_CMD_INVALID;
			pCommand->ParameterCount = 0;
			pCommand->pParameterData = nullptr;
			return result;
		}

		pCommand->Command = TranslateCommand(commandStrings[0]);

		if (pCommand->Command == MC_CONSOLE_COMMAND_CMD_INVALID) {
			pCommand->ParameterCount = 0;
			pCommand->pParameterData = nullptr;
			return MC_RESULT::FAIL_INVALID_DATA;
		}

		pCommand->ParameterCount = static_cast<unsigned char>(commandStrings.size() - 1);

		if (pCommand->ParameterCount == 0) {
			pCommand->pParameterData = nullptr;
			return MC_RESULT::OK;
		}

		commandStrings.erase(commandStrings.begin());

		pCommand->pParameterData = AllocateParameterData(commandStrings);

		return MC_RESULT::OK;
	}

	MC_RESULT MCConsoleCommandParser::ParseStrings(char *pBuffer, size_t bufferSize, std::vector<std::string>* pResult) {
		char *elementStart{ pBuffer }, *elementEnd{ pBuffer };
		for (unsigned int x = 0; x < bufferSize; x++) {
			elementEnd++;

			if (elementStart == (elementEnd - 1)) {
				if (' ' == *elementEnd) {
					elementStart = ++elementEnd;
					continue;
				}
				if ('\0' == *elementEnd) {
					return pResult->size() > 0 
						? MC_RESULT::OK 
						: MC_RESULT::FAIL_INVALID_DATA;
				}
			}
			if ('\0' == *elementEnd) {
				pResult->push_back(std::string(elementStart, elementEnd) + std::string("\0"));
				return MC_RESULT::OK;
			}
			if (' ' == *elementEnd) {
				pResult->push_back(std::string(elementStart, elementEnd) + std::string("\0"));
				elementStart = ++elementEnd;
			}
		}
		return MC_RESULT::FAIL_INVALID_DATA;
	}

	char** MCConsoleCommandParser::AllocateParameterData(const std::vector<std::string>& commandArgs) {
		std::vector<char*> ptrs;
		char** ptrArray = reinterpret_cast<char**>(_fpAllocator(sizeof(char*) * commandArgs.size()));
		for (int x = 0; x < commandArgs.size(); ++x) {
			size_t cmdSize = commandArgs[x].size() + 1;
			char *pCommand = _fpAllocator(cmdSize);
			strcpy_s(pCommand, cmdSize, commandArgs[x].c_str());
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

}