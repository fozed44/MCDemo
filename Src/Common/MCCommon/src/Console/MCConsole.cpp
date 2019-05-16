#include "MCConsole.h"
#include "../../../MCCommon/src/Data/MCThreads.h"
#include "../Global/MCCOGlobals.h"
#include "../../../MCRouter/src/Core/MCRouter.h"

#include "windows.h"
#include <vector>
namespace MC {

#pragma region ctor

	MCConsole::MCConsole(MCConsoleOutputTarget* pOutputTarget) 
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

	void MCConsole::OnProcessMessage32(MC_MESSAGE32 message) {
		switch (message.Message) {
		case MC_MESSAGE_KEY_DOWN_32:
			NewKeyHandler(message.Param);
			break;
		}
	}

	void MCConsole::OnProcessMessage128(const MC_MESSAGE128& message, const char* pData) {
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

	void MCConsole::NewKeyHandler(unsigned char ch) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		if (_pNext == _pEnd)
			return;

		if ((ch >= 0x20 && ch <= 0x7F)) { // [SPACE] - [DEL] 
			*_pNext = ch;
			_pNext++;
			_pOutputTarget->WriteChar(ch);
		} else if (ch == VK_RETURN) {
			MC_CONSOLE_COMMAND cmd;

			*_pNext = '\0';

			auto parseResult = _pCommandParser->Parse(_pKeyBuffer, CONSOLE_KEY_BUFFER_SIZE, &cmd);

			if (MC_RESULT::FAIL_INVALID_DATA == parseResult) {
				_pOutputTarget->NewLine();
				_pOutputTarget->WriteString("Unrecognized Command.");
			}

			if (MC_RESULT::OK == parseResult) {
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
		} else if (ch == VK_ESCAPE) {

			_pNext = _pKeyBuffer;

			_pOutputTarget->ClearCurrent();
			_pOutputTarget->Prompt();

		}
		else if (ch == VK_BACK) {
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

		/*MC_MESSAGE128 msg{};
		msg.Message    = MC_MESSAGE_CONSOLE_OUTPUT_128;
		msg.Visibility = MC_MESSAGE_VISIBILITY_CONSOLE;
		auto ptr = MCCOGlobals::pRouter->PushTo(msg, static_cast<unsigned short>(output.size() + 1));
		strcpy_s(ptr, output.size() + 1, output.c_str());*/
		MCCOGlobals::pRouter->PostConsoleMessage(output.c_str());
	}
}