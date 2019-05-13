#pragma once

namespace MC {

	typedef enum MC_CONSOLE_COMMAND_CMD : uint32_t {
		MC_CONSOLE_COMMAND_CMD_INVALID              = 0x00000000,
		MC_CONSOLE_COMMAND_CMD_REPORT_ROUTER_MEMORY = 0x00000001,
		MC_CONSOLE_COMMAND_CMD_WIRE_FRAME           = 0x00000002,
		MC_CONSOLE_COMMAND_CMD_ANALYZE_RESOURCE_MAN = 0x00000003
	} MC_CONSOLE_COMMAND_CMD;

	struct MC_CONSOLE_COMMAND {
		MC_CONSOLE_COMMAND_CMD Command;
		unsigned char          ParameterCount;
		char                 **pParameterData;
	};
}
