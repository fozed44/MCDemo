#pragma once

#include "DirectxMath.h"
#include "MCMath.h"

namespace MC {

	struct MCVertex1Color {
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct ObjectConstants {
		DirectX::XMFLOAT4X4 WorldViewProj = MCMath::Identity4x4();
		float  time = 1.0;
	};

	const D3D12_INPUT_ELEMENT_DESC MCVertex1Color_desc[2] = {
		{ 
			"POSITION",                                 // Semantic Name
			0,		                                    // Semantic Index
			DXGI_FORMAT_R32G32B32_FLOAT,                // Format
			0,                                          // Input Slot
			0,                                          // Aligned By Offset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input slot class
			0                                           // Instance Data Step Rate
		},
		{
			"POSITION",                                 // Semantic Name
			0,		                                    // Semantic Index
			DXGI_FORMAT_R32G32B32_FLOAT,                // Format
			0,                                          // Input Slot
			0,                                          // Aligned By Offset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input slot class
			0                                           // Instance Data Step Rate
		}
	};

}
