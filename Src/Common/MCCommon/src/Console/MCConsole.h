#pragma once

#include "windows.h"
#include "../../../MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../Console/MCConsoleOutputTarget.h"
#include "MCConsoleCommandParser.h"

#include <map>
#include <memory>

namespace MC {

	const int CONSOLE_KEY_BUFFER_SIZE = 1 * 1048;	

	class MCConsole : public MCMessageDispatchTarget {
	public:
		using tAllocator = char*(*)(size_t size);
	public: /* ctor */
		MCConsole(tAllocator ptAllocator, MCConsoleOutputTarget* pOutputTarget);
		~MCConsole();
		MCConsole(MCConsole&)              = delete;
		MCConsole(MCConsole&&)             = delete;
		MCConsole& operator= (MCConsole&)  = delete;
		MCConsole& operator= (MCConsole&&) = delete;
	public: /* MCMessageDispatchTarget */
		virtual void ProcessMessage32 (MC_MESSAGE32 message) override;
		virtual void ProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;
	public: /* Router message handlers */
		void NewKeyHandler(unsigned char vkCode);
		void ConsoleOutputHandler(const char* pData);
	private:
		char  _pKeyBuffer[CONSOLE_KEY_BUFFER_SIZE];
		char* _pNext;
		char* _pEnd;		

		std::unique_ptr<MCConsoleCommandParser> _pCommandParser;

		MCConsoleOutputTarget* _pOutputTarget;
	};

}
