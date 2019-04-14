#pragma once

#include "../../../MCRouter/src/Core/MCRouter.h"
#include <memory>

namespace MC {

	class MCCommonCore {
	public:
		MCCommonCore();
		~MCCommonCore();
		MCCommonCore(MCCommonCore&)             = delete;
		MCCommonCore(MCCommonCore&&)            = delete;
		MCCommonCore& operator=(MCCommonCore&)  = delete;
		MCCommonCore& operator=(MCCommonCore&&) = delete;

	private: /* Instances that will be copied to MCCOGlobals */
		std::unique_ptr<MCRouter> _pRouter;
	};
}

