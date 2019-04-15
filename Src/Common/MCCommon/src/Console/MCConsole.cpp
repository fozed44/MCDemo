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
			NewKeyHandler(message.Param);
			break;
		}
	}

	void MCConsole::ProcessMessage128(const MC_MESSAGE128& message, const char* pData) {
		switch (message.Message) {
		case MC_MESSAGE_CONSOLE_OUTPUT_128:
			ConsoleOutputHandler(pData);
			break;
		case MC_MESSAGE_CONSOLE_COMMAND_128:
			ConsoleCommandHandler(reinterpret_cast<const MC_CONSOLE_COMMAND*>(pData));
			break;
		}
	}

#pragma endregion

#pragma region Message Handlers

	void MCConsole::NewKeyHandler(unsigned char vkKey) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

		if (_pNext == _pEnd)
			return;

		if ((vkKey >= 0x30 && vkKey <= 0x39) // 0 - 9
		 || (vkKey >= 0x41 && vkKey <= 0x5A) // A - Z
		 ||	(VK_SPACE == vkKey)
		 || (VK_OEM_1 <= vkKey && VK_OEM_3 >= vkKey)) {
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
				msg.Message    = MC_MESSAGE_CONSOLE_COMMAND_128;
				msg.Visibility = MC_MESSAGE_VISIBILITY_ALL;
				msg.LOParam32  = cmd.Command;
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

	void MCConsole::ConsoleOutputHandler(const char* pData) {
		_pOutputTarget->WriteString(pData);
		_pOutputTarget->NewLine();
	}

	void MCConsole::ConsoleCommandHandler(const MC_CONSOLE_COMMAND* pCommand) {
		std::string output("Ack console command: ");
		output += std::string(std::to_string(pCommand->Command));
		for (int x = 0; x < pCommand->ParameterCount; ++x) {
			output += std::string(" param #") + std::to_string(x) + std::string(" ") + std::string(pCommand->pParameterData[x]);
		}

		MC_MESSAGE128 msg{};
		msg.Message = MC_MESSAGE_CONSOLE_OUTPUT_128;
		msg.Visibility = MC_MESSAGE_VISIBILITY_CONSOLE;
		auto ptr = MCCOGlobals::pRouter->PushTo(msg, output.size() + 1);
		strcpy_s(ptr, output.size() + 1, output.c_str());
	}
}