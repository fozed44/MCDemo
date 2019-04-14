#include "MCCommonCore.h"

#include "../Global/MCCOGlobals.h"

namespace MC {

	MCCommonCore::MCCommonCore() {
		MCCOGlobals::pCommonCore = this;

		_pRouter = std::make_unique<MCRouter>();
		MCCOGlobals::pRouter = _pRouter.get();
	}


	MCCommonCore::~MCCommonCore() { }

}