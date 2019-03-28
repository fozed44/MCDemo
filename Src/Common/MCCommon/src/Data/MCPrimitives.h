#pragma once

namespace MC {

	/*
		These primitives are strictly to abstract non-rendering code from rendering code.

		These primitives are do not have the fast SIMD code that is supplied for rendering primitives, therefore
		these primitives should only be used when necessary.
	*/

	struct MCFLOAT2 {
		float x;
		float y;
	};

	struct MCFLOAT3 {
		float x;
		float y;
		float z;
	};

	struct MCFLOAT4 {
		float x;
		float y;
		float z;
		float w;
	};

}
