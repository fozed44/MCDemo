#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../MCMesh.h"

#include "CubeGenerator.h"
#include "SphereGenerator.h"

#include <vector>

namespace MC { namespace GeometryGeneration {
	template <typename tVertex, int tIndex>
	MC_RESULT GenerateCubeMesh(
		const DirectX::XMFLOAT3& center,
		float width, float height, float depth,
		MCMesh<tVertex, tIndex> *pMesh
	) {
		tVertex vertexArray[8];
		MCMesh<tVertex, tIndex>::INDEX_TYPE indexArray[36];
		MC::Utilities::GenerateCube<tVertex, MCMesh<tVertex, tIndex>::INDEX_TYPE>(
			center.x, center.y, center.z,
			width, height, depth,
			vertexArray, sizeof(vertexArray),
			indexArray, sizeof(indexArray)
		);

		auto result = pMesh->LoadData(vertexArray, sizeof(vertexArray), indexArray, sizeof(indexArray));

		return result;
	}

	template <typename tVertex, int tIndex>
	MC_RESULT GenerateSphereMesh(
		float radius,
		const DirectX::XMFLOAT3& center,
		unsigned int numSubdivisions,
		MCMesh<tVertex, tIndex> *pMesh
	) {
		using INDEX_TYPE = MCMesh<tVertex, tIndex>::INDEX_TYPE;
		std::vector<tVertex>    verticies;
		std::vector<INDEX_TYPE> indicies;

		MC::Utilities::GenerateSphere(radius, center.x, center.y, center.z, &verticies, &indicies, numSubdivisions);

		auto result = pMesh->LoadData(
			&(*verticies.begin()), static_cast<unsigned>(verticies.size() * sizeof(tVertex)),
			&(*indicies.begin()) , static_cast<unsigned>(indicies.size()  * sizeof(INDEX_TYPE))
		);

		return result;
	}

}}
