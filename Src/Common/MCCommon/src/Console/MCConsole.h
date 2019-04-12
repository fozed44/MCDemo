#pragma once

#include "windows.h"
#include "../../../MCRouter/src/Core/MCMessageDispatchTarget.h"

#include <map>

namespace MC {

	const int CONSOLE_KEY_BUFFER_SIZE = 2048;

	typedef enum MC_CONSOLE_COMMAND_CMD {
		MC_CONSOLE_COMMAND_CMD_INVALID              = 0x00000000,
		MC_CONSOLE_COMMAND_CMD_REPORT_ROUTER_MEMORY = 0x00000001
	} MC_CONSOLE_COMMAND_CMD;

	struct MC_CONSOLE_COMMAND {
		MC_CONSOLE_COMMAND_CMD Command;
		unsigned char   ParameterCount;
		char **pParameterData;
	};

	class MCConsoleCommandParser {
		using tAllocator = char* (*)(unsigned int size);
	public: /* ctor */
		MCConsoleCommandParser(tAllocator allocator);
		~MCConsoleCommandParser();
		MCConsoleCommandParser(MCConsoleCommandParser&)              = delete;
		MCConsoleCommandParser(MCConsoleCommandParser&&)             = delete;
		MCConsoleCommandParser& operator= (MCConsoleCommandParser&)  = delete;
		MCConsoleCommandParser& operator= (MCConsoleCommandParser&&) = delete;
	public: /* API */
		MC_CONSOLE_COMMAND Parse(char* pBuffer, unsigned int bufferSize);
	private:
		tAllocator _fpAllocator;
		std::map<std::string, MC_CONSOLE_COMMAND_CMD> _commandMap;
	};

	class MCConsole : public MCMessageDispatchTarget {
	public: /* ctor */
		MCConsole();
		~MCConsole();
		MCConsole(MCConsole&)              = delete;
		MCConsole(MCConsole&&)             = delete;
		MCConsole& operator= (MCConsole&)  = delete;
		MCConsole& operator= (MCConsole&&) = delete;
	public: /* MCMessageDispatchTarget */
		virtual void ProcessMessage32(MC_MESSAGE32 message) override;
	public:
		void NewKey(unsigned char vkCode);
	private:
		char  _pKeyBuffer[CONSOLE_KEY_BUFFER_SIZE];
		char* _pNext;
		char* _pEnd = &_pKeyBuffer[CONSOLE_KEY_BUFFER_SIZE];		

		MCConsoleCommandParser _commandParser;
	};

}
