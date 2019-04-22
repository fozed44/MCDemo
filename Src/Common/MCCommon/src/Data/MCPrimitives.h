#pragma once
#include "DirectXMath.h"

namespace MC {

	/*
		These primitives are strictly to abstract non-rendering code from rendering code.

		These primitives are do not have the fast SIMD code that is supplied for rendering primitives, therefore
		these primitives should only be used when necessary.
	*/	

	struct MCSIMPLE_VERTEX {
		DirectX::XMFLOAT3 Position;
	};

	struct MCVERTEX {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
	};

	struct MCCOLOR_VERTEX {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT4 Color;
	};

	struct MCTEX_VERTEX {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 Tex;
	};

	typedef enum MCVERTEX_FORMAT {
		MCVERTEX_FORMAT_SIMPLE_VERTEX  = 0,
		MCVERTEX_FORMAT_VERTEX         = 1,
		MCVERTEX_FORMAT_MCCOLOR_VERTEX = 2
	} MCVERTEX_FORMAT;

	typedef enum MCBIT_DEPTH {
		MCBIT_DEPTH_0  = 0,
		MCBIT_DEPTH_1  = 1,
		MCBIT_DEPTH_8  = 8,
		MCBIT_DEPTH_16 = 16,
		MCBIT_DEPTH_32 = 32,
		MCBIT_DEPTH_64 = 64
	} MCBIT_DEPTH;
	
}
