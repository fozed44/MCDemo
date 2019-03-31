#include "MCResult.h"
#include "../Headers/Utility.h"

namespace MC {

	std::string to_string(MC_RESULT result) {
		switch (result) {
		case(MC_RESULT_OK):
				return "MC_RESULT_OK";
		case(MC_RESULT_FAIL):
			return "MC_RESULT_FAIL";
		case(MC_RESULT_FAIL_RESOURCE_LOST):
			return "MC_RESULT_FAIL_RESOURCE_LOST";
		case(MC_RESULT_FAIL_UPLOADING):
			return "MC_RESULT_FAIL_UPLOADING";
		case(MC_RESULT_FAIL_NOT_READY):
			return "MC_RESULT_FAIL_NOT_READY";
		case(MC_RESULT_FAIL_OFF):
			return "MC_RESULT_FAIL_OFF";
		case(MC_RESULT_FAIL_PAUSED):
			return "MC_RESULT_FAIL_PAUSED";
		case(MC_RESULT_ERROR):
			return "MC_RESULT_ERROR";
		default:
			MCTHROW("Invalid MC_RESULT value.");
		}
	}

}