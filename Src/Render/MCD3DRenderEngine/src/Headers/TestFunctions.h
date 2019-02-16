#pragma once

#include "D3DWrapper.h"
#include "DirectXColors.h"

namespace MCTest {
	void GenerateTestCube(
		float centerx, float centery, float centerz,
		float width, float height, float depth,
		MC::MCVertex1Color *pVertDest, size_t vertDestSize,
		std::uint16_t *pIndicies, size_t indiciesSize);
}
