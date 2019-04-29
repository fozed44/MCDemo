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
		HRenderItem LoadGeometry(const MCGEOMETRY_MESH_DESC& desc) const {
			switch (desc.Type) {
			case MCGEOMETRY_MESH_TYPE::SPHERE: /*-------------------------------------------------------------*/
			{
				std::unique_ptr<MCStaticMesh<tVertex, INDEX_FORMAT>> pMesh = std::make_unique<MCStaticMesh<tVertex, INDEX_FORMAT>>(std::string("Auto Generated Sphere"));

				// TODO:
				//	The result that is returned from GenerateSphereMesh is not checked. This result should be checked and if
				//  the result is a failure result a generic mesh (perhaps a cube) should be loaded in place of the failed
				//  mesh.

				auto result = MC::GeometryGeneration::GenerateSphereMesh(desc.Scale.x, desc.Position, desc.NumSubdivisions, pMesh.get());

				std::unique_ptr<MCRenderItem> pRenderItem = std::make_unique<MCRenderItem>(
					std::move(pMesh),
					MCREGlobals::pShaderManager->GetVertexShaderHandle<tVertex>(),
					MCREGlobals::pRSManager->GetRootSignatureHandle(STANDARD_ROOT_SIGNATURE::DEFAULT),
					MCREGlobals::pPSOManager->GetPipelineStateHandle(STANDARD_PSO::DEFAULT)
				);
				
				return _pScene->AddRenderItem(std::move(pRenderItem));
			}
			case MCGEOMETRY_MESH_TYPE::CUBE: /*-------------------------------------------------------------*/
			{
				std::unique_ptr<MCStaticMesh<tVertex, INDEX_FORMAT>> pMesh = std::make_unique<MCStaticMesh<tVertex, INDEX_FORMAT>>(std::string("Auto Generated Cube"));

				// TODO:
				//	The result that is returned from GenerateSphereMesh is not checked. This result should be checked and if
				//  the result is a failure result a generic mesh (perhaps a cube) should be loaded in place of the failed
				//  mesh.

				auto result = MC::GeometryGeneration::GenerateCubeMesh(desc.Position, desc.Scale.x, desc.Scale.y, desc.Scale.z, pMesh.get());

				std::unique_ptr<MCRenderItem> pRenderItem = std::make_unique<MCRenderItem>(
					std::move(pMesh),
					MCREGlobals::pShaderManager->GetVertexShaderHandle<tVertex>(),
					MCREGlobals::pRSManager->GetRootSignatureHandle(STANDARD_ROOT_SIGNATURE::DEFAULT),
					MCREGlobals::pPSOManager->GetPipelineStateHandle(STANDARD_PSO::DEFAULT)
				);

				return _pScene->AddRenderItem(std::move(pRenderItem));
			}
			default:
				MCTHROW("Invalid MCGEOMETRY_MESH_TYPE");
			}
			return nullptr;
		}
	private:
		MCRenderScene* _pScene;
	};

}