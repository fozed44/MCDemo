#pragma once

#include "windows.h"
#include "../../../MCRouter/src/Core/MCMessageDispatchTarget.h"
#include "../Console/MCConsoleOutputTarget.h"
#include "MCConsoleCommandParser.h"

#include <map>
#include <memory>

namespace MC {

	/* The size of the internal buffer. The console cannot create strings longer than this buffer. */
	const int CONSOLE_KEY_BUFFER_SIZE = 1 * 1048;	

	/* The job of MCConsole is to monitor the router for keyboard messages. The console keeps an internal
	   buffer that is used as the 'target' of the key-strokes. In other words MCConsole monitors the router
	   for keyboard messages that the console turns into strings. When a keyboard message for the enter key
	   is detected, the console will then pass the current buffer to MCConsoleCommandParser to create an
	   MC_CONSOLE_COMMAND object by parsing the buffer.
	*/
	class MCConsole : public MCMessageDispatchTarget {
	public: /* ctor */
		MCConsole(MCConsoleOutputTarget* pOutputTarget);
		~MCConsole();
		MCConsole(MCConsole&)              = delete;
		MCConsole(MCConsole&&)             = delete;
		MCConsole& operator= (MCConsole&)  = delete;
		MCConsole& operator= (MCConsole&&) = delete;
	public: /* MCMessageDispatchTarget */
		virtual void OnProcessMessage32 (MC_MESSAGE32 message) override;
		virtual void OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) override;
	public: /* Router message handlers */
		void NewKeyHandler        (unsigned char ch);
		void ConsoleOutputHandler (const char* pData);
		void ConsoleCommandHandler(const MC_CONSOLE_COMMAND* pCommand);
	private:
		char  _pKeyBuffer[CONSOLE_KEY_BUFFER_SIZE];
		char* _pNext;
		char* _pEnd;		

		std::unique_ptr<MCConsoleCommandParser> _pCommandParser;

		MCConsoleOutputTarget* _pOutputTarget;
	};

}
