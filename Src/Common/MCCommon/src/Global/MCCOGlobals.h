#pragma once

namespace MC {

	class MCCommonCore;
	class MCRouter;

	/*
		MCCOGlobals

		This class has no methods or instance properties. It is strictly a container for various 'singleton' objects.
		'singleton' is quoted because these objects are not true singletons. MCCommon is the 'root' object,
		and must be created normally. The rest of the objects are owned by the common object. When the common
		object is created, the common object will then create these objects which it owns. Once these owned objects
		have been created the constructor of the common object will then populate these static variables.

		NOTE:

		The reason that these 'singleton' objects are owned by the common object rather than created in true singleton
		fashion is that there are too many singletons in to many translation units, preventing a known order of destruction.
	*/
	struct MCCOGlobals {
		static MCCommonCore		*pCommonCore;
		static MCRouter			*pRouter;
	};

}

