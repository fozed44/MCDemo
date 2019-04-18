#pragma once

#include "DirectXColors.h"

namespace MC { namespace Utilities {

	template<typename tVertex, typename tIndex> 
	void GenerateTestCube(
		float centerx, float centery, float centerz,
		float width, float height, float depth,
		tVertex *pVertDest, size_t vertDestSize,
		tIndex  *pIndicies, size_t indiciesSize) {

		assert(vertDestSize == sizeof(tVertex) * 8);
		assert(indiciesSize == sizeof(tIndex) * 36);

		float xl = centerx - (width  / 2.0f);
		float xr = centerx + (width  / 2.0f);
		float yu = centery + (height / 2.0f);
		float yd = centery - (height / 2.0f);
		float zf = centerz - (depth  / 2.0f);
		float zb = centerz + (depth  / 2.0f);

		pVertDest[0].Position = DirectX::XMFLOAT3(xl, yd, zf);
		pVertDest[1].Position = DirectX::XMFLOAT3(xl, yu, zf);
		pVertDest[2].Position = DirectX::XMFLOAT3(xr, yu, zf);
		pVertDest[3].Position = DirectX::XMFLOAT3(xr, yd, zf);
		pVertDest[4].Position = DirectX::XMFLOAT3(xl, yd, zb);
		pVertDest[5].Position = DirectX::XMFLOAT3(xl, yu, zb);
		pVertDest[6].Position = DirectX::XMFLOAT3(xr, yu, zb);
		pVertDest[7].Position = DirectX::XMFLOAT3(xr, yd, zb);

		pVertDest[0].Color = DirectX::XMFLOAT4(DirectX::Colors::White);
		pVertDest[1].Color = DirectX::XMFLOAT4(DirectX::Colors::Cyan);
		pVertDest[2].Color = DirectX::XMFLOAT4(DirectX::Colors::Red);
		pVertDest[3].Color = DirectX::XMFLOAT4(DirectX::Colors::Green);
		pVertDest[4].Color = DirectX::XMFLOAT4(DirectX::Colors::Blue);
		pVertDest[5].Color = DirectX::XMFLOAT4(DirectX::Colors::Yellow);
		pVertDest[6].Color = DirectX::XMFLOAT4(DirectX::Colors::Cyan);
		pVertDest[7].Color = DirectX::XMFLOAT4(DirectX::Colors::Magenta);


		// front face
		pIndicies[0] = 0; pIndicies[1] = 1; pIndicies[2] = 2;
		pIndicies[3] = 0; pIndicies[4] = 2; pIndicies[5] = 3;

		// back face
		pIndicies[6] = 4; pIndicies[7] = 6; pIndicies[8] = 5;
		pIndicies[9] = 4; pIndicies[10] = 7; pIndicies[11] = 6;

		// left face
		pIndicies[12] = 4; pIndicies[13] = 5; pIndicies[14] = 1;
		pIndicies[15] = 4; pIndicies[16] = 1; pIndicies[17] = 0;

		// right face
		pIndicies[18] = 3; pIndicies[19] = 2; pIndicies[20] = 6;
		pIndicies[21] = 3; pIndicies[22] = 6; pIndicies[23] = 7;

		// top face
		pIndicies[24] = 1; pIndicies[25] = 5; pIndicies[26] = 6;
		pIndicies[27] = 1; pIndicies[28] = 6; pIndicies[29] = 2;

		// bottom face
		pIndicies[30] = 4; pIndicies[31] = 0; pIndicies[32] = 3;
		pIndicies[33] = 4; pIndicies[34] = 3; pIndicies[35] = 7;
	}

}}
