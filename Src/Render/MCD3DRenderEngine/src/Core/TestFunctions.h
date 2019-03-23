#pragma once

#include "MCD3D.h"
#include "DirectXColors.h"

namespace MCTest {
	void GenerateTestCube(
		float centerx, float centery, float centerz,
		float width, float height, float depth,
		MC::MCVertex1Color *pVertDest, size_t vertDestSize,
		std::uint16_t *pIndicies, size_t indiciesSize);

	void GenerateTestSphere(
		float radius,
		float centerx, float centery, float centerz,
		std::vector<MC::MCVertex1Color> *pVertDest,
		std::vector<uint16_t> *pIndDest,
		unsigned int numSubdivisions
	);
}
