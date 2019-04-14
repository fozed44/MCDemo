#include "MCConsole.h"
#include "../../../MCCommon/src/Data/MCThreads.h"
#include "../Global/MCCOGlobals.h"
#include "../../../MCRouter/src/Core/MCRouter.h"

#include "windows.h"
#include <vector>
namespace MC {

#pragma region ctor

	MCConsole::MCConsole(tAllocator ptAllocator, MCConsoleOutputTarget* pOutputTarget) 
		: _pNext{_pKeyBuffer},
		  _pEnd{&_pKeyBuffer[CONSOLE_KEY_BUFFER_SIZE]},
		  _pOutputTarget{pOutputTarget}
	{
		_pCommandParser = std::make_unique<MCConsoleCommandParser>([](size_t size) {
			return MCCOGlobals::pRouter->AllocateMessageStorage(static_cast<unsigned short>(size));
		});
		_pOutputTarget  = pOutputTarget;
	}

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

		if ((vkKey >= 0x30 && vkKey <= 0x39) || (vkKey >= 0x41 && vkKey <= 0x5A)) {
			char ascii = toascii(vkKey);
			*_pNext = ascii;
			_pNext++;
			_pOutputTarget->WriteChar(ascii);
		} else if (vkKey == VK_RETURN) {
			MC_CONSOLE_COMMAND cmd;

			*_pNext = '\0';

			auto parseResult = _pCommandParser->Parse(_pKeyBuffer, CONSOLE_KEY_BUFFER_SIZE, &cmd);

			if (MC_RESULT_FAIL_INVALID_DATA == parseResult) {
				_pOutputTarget->NewLine();
				_pOutputTarget->WriteString("Unrecognized Command.");
			}

			if (MC_RESULT_OK == parseResult) {
				MC_MESSAGE128 msg{};
				msg.Message   = MC_MESSAGE_CONSOLE_COMMAND_128;
				msg.LOParam32 = cmd.Command;
				auto ptr = MCCOGlobals::pRouter->PushTo<MC_CONSOLE_COMMAND>(msg);
				*ptr = cmd;
			}

			// Reset.
			_pNext = _pKeyBuffer;

			_pOutputTarget->NewLine();
			_pOutputTarget->Prompt();
		} else if (vkKey == VK_ESCAPE) {

			_pNext = _pKeyBuffer;

			_pOutputTarget->ClearCurrent();
			_pOutputTarget->Prompt();

		}
		else if (vkKey == VK_BACK) {
			if (_pNext > _pKeyBuffer) {
				_pNext--;
				_pOutputTarget->Backspace();
			}

		}

		

	}


}