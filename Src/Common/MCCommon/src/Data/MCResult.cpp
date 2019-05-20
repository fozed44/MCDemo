#include "MCResult.h"
#include "../Headers/Utility.h"

namespace MC {

	std::string to_string(MC_RESULT result) {
		switch (result) {
		case(MC_RESULT::OK):
				return "MC_RESULT::OK";
		case(MC_RESULT::FAIL):
			return "MC_RESULT::FAIL";
		case(MC_RESULT::FAIL_RESOURCE_LOST):
			return "MC_RESULT::FAIL_RESOURCE_LOST";
		case(MC_RESULT::FAIL_UPLOADING):
			return "MC_RESULT::FAIL_UPLOADING";
		case(MC_RESULT::FAIL_NOT_READY):
			return "MC_RESULT::FAIL_NOT_READY";
		case(MC_RESULT::FAIL_RESOURCE_LOCKED):
			return "MC_RESULT::FAIL_RESOURCE_LOCKED";
		case(MC_RESULT::FAIL_INVALID_DATA):
			return "MC_RESULT::FAIL_INVALID_DATA";
		case(MC_RESULT::FAIL_NO_OBJECT):
			return "MC_RESULT::FAIL_NO_OBJECT";
		case(MC_RESULT::FAIL_INVALID_STATE):
			return "MC_RESULT::FAIL_INVALID_STATE";
		case(MC_RESULT::MC_ERROR):
			return "MC_RESULT_ERROR";
		default:
			MCTHROW("Invalid MC_RESULT value.");
		}
	}

}