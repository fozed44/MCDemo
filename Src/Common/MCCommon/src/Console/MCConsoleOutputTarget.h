#pragma once

#include "../Data/MCResult.h"

namespace MC {

	class MCConsoleOutputTarget {
	public :
		virtual MC_RESULT DisplayConsoleOutput(char* pOutput) = 0;
	};

}