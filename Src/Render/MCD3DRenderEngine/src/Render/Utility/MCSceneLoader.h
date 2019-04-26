#pragma once

#include "../Common/MCRenderScene.h"
#include "../../../../../Common/MCCommon/src/Data/MCObjects.h"
#include "../../Mesh/Geometry/GeometryGenerator.h"

namespace MC {

	class MCSceneLoader {
	public:
		MCSceneLoader(MCRenderScene* _pScene);
		~MCSceneLoader();
		MCSceneLoader(MCSceneLoader&)              = delete;
		MCSceneLoader(MCSceneLoader&&)             = delete;
		MCSceneLoader& operator= (MCSceneLoader&)  = delete;
		MCSceneLoader& operator= (MCSceneLoader&&) = delete;

	public: /* API */
		template <typename tVertex, int INDEX_FORMAT>
		HRenderItem LoadGeometry(const MCGEOMETRY_MESH_DESC& desc) {
			switch (desc.Type) {
			case MCGEOMETRY_MESH_TYPE::SPHERE:
				std::unique_ptr<MCRenderItem>  = make_unique<MCRenderItem>();
				std::unique_ptr<MCStaticMesh<tVertex, INDEX_FORAMT>> = make_unique<MCStaticMesh<tVertex, INDEX_FORMAT>>();
				MC::GeometryGeneration::GenerateSphereMesh(desc.Scale.x, desc.Position, desc.NumSubdivisions, )
			case MCGEOMETRY_MESH_TYPE::CUBE:
			}
		}
	private:
		MCRenderScene* _pScene;
	};

}