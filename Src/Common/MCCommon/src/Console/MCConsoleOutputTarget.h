#pragma once

#include "../Data/MCResult.h"

namespace MC {

	class MCConsoleOutputTarget {
	public :
		virtual MC_RESULT WriteString(const char* pOutput) = 0;
		virtual MC_RESULT WriteChar(char output)           = 0;
		virtual MC_RESULT Backspace()                      = 0;
		virtual MC_RESULT ClearCurrent()                   = 0;
		virtual MC_RESULT Prompt()                         = 0;
		virtual MC_RESULT NewLine()                        = 0;
	};

}