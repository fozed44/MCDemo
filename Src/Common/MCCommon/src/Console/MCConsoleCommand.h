#pragma once

namespace MC {

	/* Define the console command codes that can be recognized by the command parser. The job of
	   the command parser is to map hard-coded strings to these codes. */
	typedef enum MC_CONSOLE_COMMAND_CMD : uint32_t {
		MC_CONSOLE_COMMAND_CMD_INVALID              = 0x00000000,
		MC_CONSOLE_COMMAND_CMD_REPORT_ROUTER_MEMORY = 0x00000001,
		MC_CONSOLE_COMMAND_CMD_WIRE_FRAME           = 0x00000002,
		MC_CONSOLE_COMMAND_CMD_ANALYZE_RESOURCE_MAN = 0x00000003,
		MC_CONSOLE_COMMAND_CMD_RENDERER             = 0x00000004,
		MC_CONSOLE_COMMAND_CMD_ANALYZE			    = 0x00000005
	} MC_CONSOLE_COMMAND_CMD;

	/* Pack the command identifier along with the parameters parsed by the command parser. */
	struct MC_CONSOLE_COMMAND {
		MC_CONSOLE_COMMAND_CMD Command;
		unsigned char          ParameterCount;
		char                 **pParameterData;
	};
}
