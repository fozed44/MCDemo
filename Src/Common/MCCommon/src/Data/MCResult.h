#pragma once

#include <string>

/*
	Define results that can be returned by MC methods.
*/

namespace MC {

	typedef enum class MC_RESULT : unsigned int {
		OK					  = 0x00000000,
		FAIL				  = 0x08000000,
		FAIL_RESOURCE_LOST	  = 0x08000001,
		FAIL_UPLOADING        = 0x08000002,
		FAIL_NOT_READY        = 0x08000003,
		FAIL_OFF              = 0x08000004,
		FAIL_PAUSED           = 0x08000005,
		FAIL_RESOURCE_LOCKED  = 0x08000006,
		FAIL_INVALID_DATA     = 0x08000007,
		MC_ERROR    		  = 0x80000000,
		ERROR_OUT_OF_MEMORY   = 0x80000001,
	} MC_RESULT;

	std::string to_string(MC_RESULT result);
}