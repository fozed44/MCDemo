#include "../Headers/TestFunctions.h"

namespace MCTest {
	
	void GenerateTestCube(
		float centerx, float centery, float centerz,
		float width, float height, float depth,
		MC::MCVertex1Color *pVertDest, size_t vertDestSize,
		std::uint16_t *pIndicies, size_t indiciesSize) {

		assert(vertDestSize == sizeof(MC::MCVertex1Color) * 8);
		assert(indiciesSize == sizeof(std::uint16_t) * 36);

		float xl = centerx - (width / 2.0f);
		float xr = centerx + (width / 2.0f);
		float yu = centery + (height / 2.0f);
		float yd = centery - (height / 2.0f);
		float zf = centerz - (depth / 2.0f);
		float zb = centerz + (depth / 2.0f);

		pVertDest[0] = { DirectX::XMFLOAT3(xl, yd, zf), DirectX::XMFLOAT4(DirectX::Colors::White) };
		pVertDest[1] = { DirectX::XMFLOAT3(xl, yu, zf), DirectX::XMFLOAT4(DirectX::Colors::Cyan) };
		pVertDest[2] = { DirectX::XMFLOAT3(xr, yu, zf), DirectX::XMFLOAT4(DirectX::Colors::Red) };
		pVertDest[3] = { DirectX::XMFLOAT3(xr, yd, zf), DirectX::XMFLOAT4(DirectX::Colors::Green) };

		pVertDest[4] = { DirectX::XMFLOAT3(xl, yd, zb), DirectX::XMFLOAT4(DirectX::Colors::Blue) };
		pVertDest[5] = { DirectX::XMFLOAT3(xl, yu, zb), DirectX::XMFLOAT4(DirectX::Colors::Yellow) };
		pVertDest[6] = { DirectX::XMFLOAT3(xr, yu, zb), DirectX::XMFLOAT4(DirectX::Colors::Cyan) };
		pVertDest[7] = { DirectX::XMFLOAT3(xr, yd, zb), DirectX::XMFLOAT4(DirectX::Colors::Magenta) };

		
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

	void Subdivide(std::vector<MC::MCVertex1Color> *pVerts, std::vector<uint16_t> *pIndicies)
	{
		std::vector<MC::MCVertex1Color> tempVerts;
		std::vector<uint16_t>           tempIndicies;

		for (int i = 0; i < pVerts->size(); ++i)
			tempVerts.push_back((*pVerts)[i]);

		for (int i = 0; i < pIndicies->size(); ++i)
			tempIndicies.push_back((*pIndicies)[i]);

		pVerts->clear();
		pIndicies->clear();

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		UINT32 numTris = (UINT32)tempIndicies.size() / 3;
		for (UINT32 i = 0; i < numTris; ++i)
		{
			MC::MCVertex1Color v0 = (tempVerts)[(tempIndicies)[i * 3 + 0]];
			MC::MCVertex1Color v1 = (tempVerts)[(tempIndicies)[i * 3 + 1]];
			MC::MCVertex1Color v2 = (tempVerts)[(tempIndicies)[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			MC::MCVertex1Color m0 = { DirectX::XMFLOAT3(
										(v0.Position.x + v1.Position.x) / 2.0f,
										(v0.Position.y + v1.Position.y) / 2.0f,
										(v0.Position.z + v1.Position.z) / 2.0f
									  ),
									  DirectX::XMFLOAT4(
										MC::MCMath::Min((v0.Color.x + v1.Color.x) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.y + v1.Color.y) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.z + v1.Color.z) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.w + v1.Color.w) / 2.0f, 1.0f)
									  )
			};
			MC::MCVertex1Color m1 = { DirectX::XMFLOAT3(
										(v1.Position.x + v2.Position.x) / 2.0f,
										(v1.Position.y + v2.Position.y) / 2.0f,
										(v1.Position.z + v2.Position.z) / 2.0f
									  ),
									  DirectX::XMFLOAT4(
										MC::MCMath::Min((v1.Color.x + v2.Color.x) / 2.0f, 1.0f),
										MC::MCMath::Min((v1.Color.y + v2.Color.y) / 2.0f, 1.0f),
										MC::MCMath::Min((v1.Color.z + v2.Color.z) / 2.0f, 1.0f),
										MC::MCMath::Min((v1.Color.w + v2.Color.w) / 2.0f, 1.0f)
									  )
			};
			MC::MCVertex1Color m2 ={ DirectX::XMFLOAT3(
										(v0.Position.x + v2.Position.x) / 2.0f,
										(v0.Position.y + v2.Position.y) / 2.0f,
										(v0.Position.z + v2.Position.z) / 2.0f
									  ),
									  DirectX::XMFLOAT4(
										MC::MCMath::Min((v0.Color.x + v2.Color.x) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.y + v2.Color.y) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.z + v2.Color.z) / 2.0f, 1.0f),
										MC::MCMath::Min((v0.Color.w + v2.Color.w) / 2.0f, 1.0f)
									  )
			};

			//
			// Add new geometry.
			//

			pVerts->push_back(v0); // 0
			pVerts->push_back(v1); // 1
			pVerts->push_back(v2); // 2
			pVerts->push_back(m0); // 3
			pVerts->push_back(m1); // 4
			pVerts->push_back(m2); // 5

			pIndicies->push_back(i * 6 + 0);
			pIndicies->push_back(i * 6 + 3);
			pIndicies->push_back(i * 6 + 5);

			pIndicies->push_back(i * 6 + 3);
			pIndicies->push_back(i * 6 + 4);
			pIndicies->push_back(i * 6 + 5);

			pIndicies->push_back(i * 6 + 5);
			pIndicies->push_back(i * 6 + 4);
			pIndicies->push_back(i * 6 + 2);

			pIndicies->push_back(i * 6 + 3);
			pIndicies->push_back(i * 6 + 1);
			pIndicies->push_back(i * 6 + 4);
		}
	}

	void GenerateTestSphere(
		float radius,
		float centerx, float centery, float centerz,
		std::vector<MC::MCVertex1Color> *pVertDest,
		std::vector<uint16_t> *pIndDest,
		unsigned int numSubdivisions
	) {
		numSubdivisions = MC::MCMath::Min(numSubdivisions, 6u);

		const float X = 0.525731f;
		const float Z = 0.850651f;

		DirectX::XMFLOAT3 pos[12] =
		{
			DirectX::XMFLOAT3 ( -X,    0.0f,  Z ), DirectX::XMFLOAT3 (   X, 0.0f,  Z ),
			DirectX::XMFLOAT3 ( -X,    0.0f, -Z ), DirectX::XMFLOAT3 (   X, 0.0f, -Z ),
			DirectX::XMFLOAT3 (0.0f,     Z,   X ), DirectX::XMFLOAT3 ( 0.0f,  Z,  -X ),
			DirectX::XMFLOAT3 (0.0f,    -Z,   X ), DirectX::XMFLOAT3 ( 0.0f, -Z,  -X ),
			DirectX::XMFLOAT3 (  Z,      X, 0.0f), DirectX::XMFLOAT3 (  -Z,   X, 0.0f),
			DirectX::XMFLOAT3 (  Z,     -X, 0.0f), DirectX::XMFLOAT3 (  -Z,  -X, 0.0f)
		};

		UINT32 k[60] =
		{
			1, 4, 0, 4, 9,0, 4, 5,9, 8,5, 4, 1, 8,4,
			1, 10,8, 10,3,8, 8, 3,5, 3,2, 5, 3, 7,2,
			3, 10,7, 10,6,7, 6,11,7, 6,0,11, 6, 1,0,
			10,1, 6, 11,0,9, 2,11,9, 5,2, 9, 11,2,7
		};

		pIndDest->assign(&k[0], &k[60]);

		for (int i = 0; i < 12; ++i)
			pVertDest->push_back({ pos[i], DirectX::XMFLOAT4(MC::MCMath::RandF(), MC::MCMath::RandF(), MC::MCMath::RandF(), MC::MCMath::RandF()) });

		

		for (int i = 0; i < numSubdivisions; ++i) {
			Subdivide(pVertDest, pIndDest);
			if (i < 3) {
				for (int x = 0; x < pVertDest->size(); ++x) {
					(*pVertDest)[x].Color.x = MC::MCMath::RandF();
					(*pVertDest)[x].Color.y = MC::MCMath::RandF();
					(*pVertDest)[x].Color.z = MC::MCMath::RandF();
					(*pVertDest)[x].Color.w = MC::MCMath::RandF();
				}
			}
		}

		for (int i = 0; i < pVertDest->size(); ++i) {

			DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*pVertDest)[i].Position));
			DirectX::XMFLOAT3 r;
			DirectX::XMStoreFloat3(&r, n);

			(*pVertDest)[i].Position.x = r.x * radius + centerx;
			(*pVertDest)[i].Position.y = r.y * radius + centery;
			(*pVertDest)[i].Position.z = r.z * radius + centerz;


		}

	}
	
}