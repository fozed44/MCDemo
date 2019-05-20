#pragma once

namespace MC {

	/* Define the console command codes that can be recognized by the command parser. The job of
	   the command parser is to map hard-coded strings to these codes. */
	typedef enum class MC_CONSOLE_COMMAND_CMD : uint32_t {
		INVALID    = 0x00000000,
		REPORT     = 0x00000001,
		WIRE_FRAME = 0x00000002,
		RENDER     = 0x00000004
	} MC_CONSOLE_COMMAND_CMD;

	/* Pack the command identifier along with the parameters parsed by the command parser. */
	struct MC_CONSOLE_COMMAND {
		MC_CONSOLE_COMMAND_CMD Command;
		unsigned char          ParameterCount;
		char                 **pParameterData;
	};
}
