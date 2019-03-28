#pragma once

/*
	Define results that can be returned by MC methods.
*/

namespace MC {

	enum MC_RESULT {
		MC_RESULT_OK					= 0x00000000,
		MC_RESULT_FAIL					= 0x08000000,
		MC_RESULT_FAIL_RESOURCE_LOST	= 0x08000001,
		MC_RESULT_FAIL_UPLOADING        = 0x08000002,
		MC_RESULT_ERROR					= 0x80000000,
	};

}