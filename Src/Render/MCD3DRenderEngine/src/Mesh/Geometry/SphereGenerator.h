#pragma once

#include "DirectXColors.h"
#include <vector>


namespace MC { namespace Utilities {
	
	template<typename tVertex>
	int GetIndexByPosition(const std::vector<tVertex> *pVerts, const DirectX::XMFLOAT3 *pLocation) {
		for (int x = 0; x < (int)pVerts->size(); ++x) {
			auto currentPosition = (*pVerts)[x].Position;
			if(MC::MCMath::Abs(pLocation->x - currentPosition.x) < 0.001
			&& MC::MCMath::Abs(pLocation->y - currentPosition.y) < 0.001
			&& MC::MCMath::Abs(pLocation->z - currentPosition.z) < 0.001)
				return x;
		}
		return -1;
	}
	
	template<typename tVertex,  typename tIndex>
	void Subdivide(std::vector<tVertex> *pVerts, std::vector<tIndex> *pIndicies)
	{
		std::vector<tVertex> tempVerts;
		std::vector<tIndex>  tempIndicies;
	
		for (int i = 0; i < (int)pVerts->size(); ++i)
			tempVerts.push_back((*pVerts)[i]);
	
		for (int i = 0; i < (int)pIndicies->size(); ++i)
			tempIndicies.push_back((*pIndicies)[i]);
	
		pVerts->clear();
		pIndicies->clear();
	
		//       v1
		//       *
		//      /  \
		//     /     \
		//  m0*-----*m1
		//   / \       / \
		//  /    \    /    \
		// *-----*-----*
		// v0    m2     v2

		UINT32 numTris = (UINT32)tempIndicies.size() / 3;
		for (UINT32 i = 0; i < numTris; ++i)
		{
			tVertex v0 = (tempVerts)[(tempIndicies)[i * 3 + 0]];
			tVertex v1 = (tempVerts)[(tempIndicies)[i * 3 + 1]];
			tVertex v2 = (tempVerts)[(tempIndicies)[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			tVertex m0 = {};
			m0.Position = DirectX::XMFLOAT3(
				(v0.Position.x + v1.Position.x) / 2.0f,
				(v0.Position.y + v1.Position.y) / 2.0f,
				(v0.Position.z + v1.Position.z) / 2.0f
			);
			m0.Color = DirectX::XMFLOAT4(
				MC::MCMath::Min((v0.Color.x + v1.Color.x) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.y + v1.Color.y) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.z + v1.Color.z) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.w + v1.Color.w) / 2.0f, 1.0f)
			);

			tVertex m1 = {};
			m1.Position = DirectX::XMFLOAT3(
				(v1.Position.x + v2.Position.x) / 2.0f,
				(v1.Position.y + v2.Position.y) / 2.0f,
				(v1.Position.z + v2.Position.z) / 2.0f
			);
			m1.Color = DirectX::XMFLOAT4(
				MC::MCMath::Min((v1.Color.x + v2.Color.x) / 2.0f, 1.0f),
				MC::MCMath::Min((v1.Color.y + v2.Color.y) / 2.0f, 1.0f),
				MC::MCMath::Min((v1.Color.z + v2.Color.z) / 2.0f, 1.0f),
				MC::MCMath::Min((v1.Color.w + v2.Color.w) / 2.0f, 1.0f)
			);

			tVertex m2 = {};
			m2.Position = DirectX::XMFLOAT3(
				(v0.Position.x + v2.Position.x) / 2.0f,
				(v0.Position.y + v2.Position.y) / 2.0f,
				(v0.Position.z + v2.Position.z) / 2.0f
			);
			m2.Color = DirectX::XMFLOAT4(
				MC::MCMath::Min((v0.Color.x + v2.Color.x) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.y + v2.Color.y) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.z + v2.Color.z) / 2.0f, 1.0f),
				MC::MCMath::Min((v0.Color.w + v2.Color.w) / 2.0f, 1.0f)
			);

			//
			// Add new geometry.
			//
			uint16_t i0, i1, i2, i3, i4, i5;
			int index;
			
			index = GetIndexByPosition(pVerts, &(v0.Position));
			if (index == -1) {
				i0 = (uint16_t)pVerts->size();
				pVerts->push_back(v0);
			}
			else {
				i0 = index;
			}

			index = GetIndexByPosition(pVerts, &(v1.Position));
			if (index == -1) {
				i1 = (uint16_t)pVerts->size();
				pVerts->push_back(v1);
			}
			else {
				i1 = index;
			}

			index = GetIndexByPosition(pVerts, &(v2.Position));
			if (index == -1) {
				i2 = (uint16_t)pVerts->size();
				pVerts->push_back(v2);
			}
			else {
				i2 = index;
			}

			index = GetIndexByPosition(pVerts, &(m0.Position));
			if (index == -1) {
				i3 = (uint16_t)pVerts->size();
				pVerts->push_back(m0);
			}
			else {
				i3 = index;
			}

			index = GetIndexByPosition(pVerts, &(m1.Position));
			if (index == -1) {
				i4 = (uint16_t)pVerts->size();
				pVerts->push_back(m1);
			}
			else {
				i4 = index;
			}

			index = GetIndexByPosition(pVerts, &(m2.Position));
			if (index == -1) {
				i5 = (uint16_t)pVerts->size();
				pVerts->push_back(m2);
			}
			else {
				i5 = index;
			}


			pIndicies->push_back(i0);
			pIndicies->push_back(i3);
			pIndicies->push_back(i5);

			pIndicies->push_back(i3);
			pIndicies->push_back(i4);
			pIndicies->push_back(i5);

			pIndicies->push_back(i5);
			pIndicies->push_back(i4);
			pIndicies->push_back(i2);

			pIndicies->push_back(i3);
			pIndicies->push_back(i1);
			pIndicies->push_back(i4);
		}
	}

	template<typename tVertex, typename tIndex>
	void GenerateSphere(
		float radius,
		float centerx, float centery, float centerz,
		std::vector<tVertex> *pVertDest,
		std::vector<tIndex> *pIndDest,
		unsigned int numSubdivisions
	) {
		numSubdivisions = MC::MCMath::Min(numSubdivisions, 6u);

		const float X = 0.525731f;
		const float Z = 0.850651f;
		const float Y = 0.0;

		DirectX::XMFLOAT3 pos[12] =
		{
			DirectX::XMFLOAT3 ( -X,    Y,  Z ), DirectX::XMFLOAT3 (  X,  Y,  Z ),
			DirectX::XMFLOAT3 ( -X,    Y, -Z ), DirectX::XMFLOAT3 (  X,  Y, -Z ),
			DirectX::XMFLOAT3 (  Y,    Z,  X ), DirectX::XMFLOAT3 (  Y,  Z, -X ),
			DirectX::XMFLOAT3 (  Y,   -Z,  X ), DirectX::XMFLOAT3 (  Y, -Z, -X ),
			DirectX::XMFLOAT3 (  Z,    X,  Y ), DirectX::XMFLOAT3 ( -Z,  X,  Y ),
			DirectX::XMFLOAT3 (  Z,   -X,  Y ), DirectX::XMFLOAT3 ( -Z, -X,  Y )
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
		
		for (unsigned int i = 0; i < numSubdivisions; ++i) {
			Subdivide(pVertDest, pIndDest);
			if (i < 3) {
				for (int x = 0; x < (int)pVertDest->size(); ++x) {
					(*pVertDest)[x].Color.x = MC::MCMath::RandF();
					(*pVertDest)[x].Color.y = MC::MCMath::RandF();
					(*pVertDest)[x].Color.z = MC::MCMath::RandF();
					(*pVertDest)[x].Color.w = MC::MCMath::RandF();
				}
			}
		}

		for (int i = 0; i < (int)pVertDest->size(); ++i) {

			DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*pVertDest)[i].Position));
			DirectX::XMFLOAT3 r;
			DirectX::XMStoreFloat3(&r, n);

			(*pVertDest)[i].Position.x = r.x * radius + centerx;
			(*pVertDest)[i].Position.y = r.y * radius + centery;
			(*pVertDest)[i].Position.z = r.z * radius + centerz;
		}
	}
}}