#pragma once

#include "../Headers/GlobalSwitches.h"
#include "../Data/MCResult.h"
#include "MCConsoleCommand.h"

#include <vector>
#include <string>
#include <map>


namespace MC {

	class MCConsoleCommandParser {
		using tAllocator = char* (*)(size_t size);
	public: /* ctor */
		MCConsoleCommandParser(tAllocator allocator);
		~MCConsoleCommandParser();
		MCConsoleCommandParser(MCConsoleCommandParser&) = delete;
		MCConsoleCommandParser(MCConsoleCommandParser&&) = delete;
		MCConsoleCommandParser& operator= (MCConsoleCommandParser&) = delete;
		MCConsoleCommandParser& operator= (MCConsoleCommandParser&&) = delete;
	public: /* API */
		MC_RESULT Parse(char* pBuffer, size_t bufferSize, MC_CONSOLE_COMMAND* pCommand);
	private:
		tAllocator _fpAllocator;
		MC_RESULT ParseStrings(char* pBuffer, size_t bufferSize, std::vector<std::string>* pResult);
		char**    AllocateParameterData(const std::vector<std::string>& commandArgs);
		MC_CONSOLE_COMMAND_CMD TranslateCommand(const std::string& command);
		std::map<std::string, MC_CONSOLE_COMMAND_CMD> _commandMap;
	};

}
