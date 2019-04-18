#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../MCMesh.h"

#include "CubeGenerator.h"

namespace MC { namespace GeometryGeneration {
	template <typename tVertex, int tIndex>
	MC_RESULT GenerateCubeMesh(
		const DirectX::XMFLOAT3& center,
		float width, float height, float depth,
		MCMesh<tVertex, tIndex> *pMesh
	) {
		tVertex vertexArray[8];
		MCMesh<tVertex, tIndex>::INDEX_TYPE indexArray[36];
		MC::Utilities::GenerateTestCube<tVertex, MCMesh<tVertex, tIndex>::INDEX_TYPE>(
			center.x, center.y, center.z,
			width, height, depth,
			vertexArray, sizeof(vertexArray),
			indexArray, sizeof(indexArray)
		);

		pMesh->LoadData(vertexArray, sizeof(vertexArray), indexArray, sizeof(indexArray));

		return MC_RESULT::OK;
	}

	template <typename tVertex, int tIndex>
	MC_RESULT GenerateSphereMesh(
		float radius,
		const DirectX::XMFLOAT3& center,
		unsigned int numSubdivisions,
		MCMesh<tVertex, tIndex> *pMesh
	) {
		return MC_RESULT::OK;
	}

}}
