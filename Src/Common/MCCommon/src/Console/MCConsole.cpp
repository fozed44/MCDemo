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

	MCConsoleCommandParser::Parse(char *pBuffer, unsigned int bufferSize) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		std::vector<std::string> parsedElements;
		unsigned int elementStart{ 0 }, elementEnd{ 0 };
		std::string currentElement;
		for (unsigned int x = 0; x < CONSOLE_KEY_BUFFER_SIZE; x++) {
			char nextChar = _pKeyBuffer[x];

			currentElement += nextChar;

			if ('-' == nextChar)

				if (!nextChar)
					break;
		}
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