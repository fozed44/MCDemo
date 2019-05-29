#pragma once

#include "../Core/MCD3D12Core.h"
#include "../Core/MCD3D12RenderUtilities.h"
#include "../Memory/MCResourceManager.h"

#include <unordered_map>

using Microsoft::WRL::ComPtr;

namespace MC {

	template<int iIndexFormat>
	class IndexFormat {
	};

	template <>
	class IndexFormat<DXGI_FORMAT_R16_UINT> {
		unsigned short TYPE;
	};

	template <>
	class IndexFormat<DXGI_FORMAT_R32_UINT> {
		unsigned int TYPE;
	};

	struct MCSubMesh {
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		UINT BaseVertexLocation = 0;
	};

	/*
		MCMesh is the base class for all meshes.
	*/
	class MCIMesh {
	public:
		virtual ~MCIMesh() {}
	public:
		virtual const std::string& Name() const noexcept = 0;
	public:
		virtual void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) = 0;
	public:
		virtual const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView() const noexcept = 0;
		virtual const D3D12_INDEX_BUFFER_VIEW*  IndexBufferView()  const noexcept = 0;
	public:
		virtual void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const = 0;
		virtual void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) = 0;
	public:
		virtual void Draw(ID3D12GraphicsCommandList *pCommandList) = 0;
		virtual void Draw(const std::string& meshName, ID3D12GraphicsCommandList *pCommandList) = 0;
	};

	/*
		MCMesh is the base class for all meshes.
	*/
	template <typename TVERTEX_TYPE, int INDEX_FORMAT>
	class MCMesh : public MCIMesh {
		static_assert(INDEX_FORMAT == DXGI_FORMAT_R16_UINT || INDEX_FORMAT == DXGI_FORMAT_R32_UINT, "Invalid DXGI_FORMAT");
	public: /* INCEX_TYPE */
		using INDEX_TYPE = decltype(IndexFormat<INDEX_FORMAT>::TYPE);
	public:
		MCMesh(std::string &name) : _name(name) {}
		MCMesh(MCMesh&)             = delete;
		MCMesh& operator= (MCMesh&) = delete;
		virtual ~MCMesh() {}
	public:
		const std::string& Name() const noexcept override { return _name; }
	public:
		MC_RESULT LoadData(TVERTEX_TYPE *pVert, UINT vSize, INDEX_TYPE *pIndicies, UINT iSize) {
			// Store the meta information about this mesh.
			_vertexByteStride     = sizeof(TVERTEX_TYPE);
			_vertexBufferByteSize = vSize;
			_indexBufferByteSize  = iSize;

			auto createResourcesResult = CreateResources(pVert, vSize, pIndicies, iSize, &_hVertexBuffer, &_hIndexBuffer);
			if (MC_RESULT::OK != createResourcesResult)
				return createResourcesResult;

			auto vertexBufferResource = MCResourceManager::Instance()->GetStaticResourceSync(_hVertexBuffer);
			vertexBufferResource->SetName(s2ws(Name() + std::string(" gpu vertex buffer")).c_str());

			auto indexBufferResource = MCResourceManager::Instance()->GetStaticResourceSync(_hIndexBuffer);
			indexBufferResource->SetName(s2ws(Name() + std::string(" gpu index buffer")).c_str());

			// Next we add the entire mesh as a subMesh, allowing the client to call DrawSubMesh using the name of this mesh
			// to draw the whole mesh.
			AddSubMesh(Name().c_str(), iSize / sizeof(INDEX_TYPE), 0, 0);

			_vertexBufferView.BufferLocation = vertexBufferResource->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes  = _vertexByteStride;
			_vertexBufferView.SizeInBytes    = _vertexBufferByteSize;

			_indexBufferView.BufferLocation = indexBufferResource->GetGPUVirtualAddress();
			_indexBufferView.Format         = _indexFormat;
			_indexBufferView.SizeInBytes    = _indexBufferByteSize;

			return MC_RESULT::OK;
		}

		void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) override {
			MCSubMesh subMesh;
			subMesh.IndexCount = indexCount;
			subMesh.StartIndexLocation = startIndexLocation;
			subMesh.BaseVertexLocation = baseVertexLocation;
			_subMeshes[pMeshName] = subMesh;
		}
	public:

		const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView() const noexcept override { return &_vertexBufferView; }
		const D3D12_INDEX_BUFFER_VIEW*  IndexBufferView()  const noexcept override { return &_indexBufferView; }

	public:

		void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const override {
			pCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			pCommandList->IASetIndexBuffer(&_indexBufferView);
		}

		void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) override {
			auto subMesh = _subMeshes[pMeshName];
			pCommandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}

	public:

		void Draw(ID3D12GraphicsCommandList *pCommandList) override {
			Draw(_subMeshes.begin()->first, pCommandList);
		}

		void Draw(const std::string& meshName, ID3D12GraphicsCommandList *pCommandList) override {
			SetIABuffers(pCommandList);
			DrawSubMesh(meshName.c_str(), pCommandList);
		}
	private:
		std::string _name;

	protected:
		virtual MC_RESULT CreateResources(
			TVERTEX_TYPE *pVert,
			UINT vSize,
			INDEX_TYPE *pIndicies,
			UINT iSize,
			MCResourceManager::HandleType* hVertexBuffer,
			MCResourceManager::HandleType* hIndexBuffer
		) = 0;
	protected:
		std::unordered_map<std::string, MCSubMesh> _subMeshes;

	protected:
		MCResourceManager::HandleType _hVertexBuffer;
		MCResourceManager::HandleType _hIndexBuffer;

	protected:
		UINT        _vertexByteStride;
		UINT        _vertexBufferByteSize;
		DXGI_FORMAT _indexFormat = static_cast<DXGI_FORMAT>(INDEX_FORMAT);
		UINT        _indexBufferByteSize;

	protected:
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
	};	

	/*
		MCStaticMesh is a static mesh, (D3D12_HEAP_TYPE_DEFAULT) with a vertex type of VERTEX_TYPE and an
		index type of either DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT
	*/
	template <typename TVERTEX_TYPE, int INDEX_FORMAT>
	class MCStaticMesh : public MCMesh<TVERTEX_TYPE, INDEX_FORMAT> {
	public:
		MCStaticMesh(std::string& name) : MCMesh(name) {}
		~MCStaticMesh() {}
	protected:
		virtual MC_RESULT CreateResources(
			TVERTEX_TYPE *pVert,
			UINT vSize,
			INDEX_TYPE *pIndicies,
			UINT iSize,
			MCResourceManager::HandleType* hVertexBuffer,
			MCResourceManager::HandleType* hIndexBuffer
		) override {
			*hVertexBuffer = std::move(MCResourceManager::Instance()->CreateStaticBufferSync(pVert, vSize));
			*hIndexBuffer  = std::move(MCResourceManager::Instance()->CreateStaticBufferSync(pIndicies, iSize));
			SET_MCRESUORCE_DBG_NAME(*hVertexBuffer, std::string(std::string(Name()) + " vertex buffer").c_str());
			SET_MCRESUORCE_DBG_NAME(*hIndexBuffer,  std::string(std::string(Name()) + " index buffer").c_str());
			return MC_RESULT::OK;
		}
	};

}