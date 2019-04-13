#pragma once

#include <string>

/*
	Define results that can be returned by MC methods.
*/

namespace MC {

	typedef enum MC_RESULT {
		MC_RESULT_OK					= 0x00000000,
		MC_RESULT_FAIL					= 0x08000000,
		MC_RESULT_FAIL_RESOURCE_LOST	= 0x08000001,
		MC_RESULT_FAIL_UPLOADING        = 0x08000002,
		MC_RESULT_FAIL_NOT_READY        = 0x08000003,
		MC_RESULT_FAIL_OFF              = 0x08000004,
		MC_RESULT_FAIL_PAUSED           = 0x08000005,
		MC_RESULT_FAIL_RESOURCE_LOCKED  = 0x08000006,
		MC_RESULT_FAIL_INVALID_DATA     = 0x08000007,
		MC_RESULT_ERROR					= 0x80000000,
		MC_RESULT_ERROR_OUT_OF_MEMORY   = 0x80000001,
	} MC_RESULT;

	std::string to_string(MC_RESULT result);
}