#pragma once

#include "MCD3D12Core.h"
#include "MCD3D12RenderUtilities.h"
#include <unordered_map>

using Microsoft::WRL::ComPtr;

namespace MC {

	template <typename TVERTEX_TYPE> class MCStaticMesh16;
	template <typename TVERTEX_TYPE> class MCDynamicMesh16;

	struct MCSubMesh {
		UINT IndexCount         = 0;
		UINT StartIndexLocation = 0;
		UINT BaseVertexLocation = 0;		
	};

	template <typename TVERTEX_TYPE>
	class MCStaticMeshUploaderDisposer {
	public:
		MCStaticMeshUploaderDisposer(MCStaticMesh16<TVERTEX_TYPE> *pMesh)
			: _pMesh(pMesh) {}
		MCStaticMeshUploaderDisposer(MCStaticMeshUploaderDisposer&)            = delete;
		MCStaticMeshUploaderDisposer& operator=(MCStaticMeshUploaderDisposer&) = delete;
		MCStaticMeshUploaderDisposer(MCStaticMeshUploaderDisposer&& o) {
			this->_pMesh = o._pMesh;
			o._pMesh = nullptr;
		}
		~MCStaticMeshUploaderDisposer() {
			if (_pMesh)
				_pMesh->DisposeUploaders();
		}
	private:
		MCStaticMesh16<TVERTEX_TYPE> *_pMesh;
	};

	/*
		MCStaticMesh16 is a static mesh, (D3D12_HEAP_TYPE_DEFAULT) with a vertex type of VERTEX_TYPE and an
		index type of DXGI_FORMAT_R16_UINT
	*/
	template <typename TVERTEX_TYPE>
	class MCStaticMesh16 {
	public:
		MCStaticMesh16(std::string& name) {
			_name = name;
		}
		MCStaticMesh16(const char* pName) {
			_name = std::string(pName);
		}
		MCStaticMesh16(MCStaticMesh16&)            = delete;
		MCStaticMesh16& operator=(MCStaticMesh16&) = delete;
		~MCStaticMesh16() {}

		MCStaticMeshUploaderDisposer<TVERTEX_TYPE> Upload(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, TVERTEX_TYPE *pVert, UINT vSize, unsigned short *pIndicies, UINT iSize) {
			// For the time being, this Upload method does not attempt to check the current status of the mesh to see if
			// data has already been loaded. Since this check is not being done, if Upload is called on a mesh that has already
			// been loaded with data, the data that already exists at the time the method is called will not be properly destroyed.

			// For this reason, we are not allowing Upload to be called more that once on a Mesh. We are going to ensure
			// this by asserting that the _uploaderDisposed flag is still false.
			assert(!_uploadersDisposed);

			// Create the disposer that will be returned to the caller.
			MCStaticMeshUploaderDisposer<TVERTEX_TYPE> disposer(this);

			// Store the meta information about this mesh.
			_vertexByteStride     = sizeof(TVERTEX_TYPE);
			_vertexBufferByteSize = vSize;
			_indexBufferByteSize  = iSize;

			// Create the vertex and index buffer.

			// Note: 
			//	Remember that CreateDefaultBuffer is an async operation. The method only adds the commands to copy the mesh
			//  data into the GPU buffers to the command list. Also remember that the resulting GPU buffers are read only and
			//  the vertex data must be copied to these buffers via the upload buffers (the output parameters passed to this method)
			//  and these upload buffers must be kept alive until the command list has executed the copy (enter the disposer returned
			//  by this method).
			_vertexBufferGPU      = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pVert,     vSize, _vertexUploadBuffer);
			_indexBufferGPU       = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pIndicies, iSize, _indexUploadBuffer);

			// Set the name of the GPU buffers.
			_vertexBufferGPU->SetName(s2ws(_name + std::string(" gpu vertex buffer")).c_str());
			_indexBufferGPU ->SetName(s2ws(_name + std::string(" gpu index buffer")).c_str());

			// Set the name of the Upload buffers (even though) they are temporary.
			_vertexUploadBuffer->SetName(s2ws(_name + std::string(" vertex upload buffer")).c_str());
			_indexUploadBuffer ->SetName(s2ws(_name + std::string(" index upload buffer")).c_str());

			// Next we add the entire mesh as a subMesh, allowing the client to call DrawSubMesh using the name of this mesh
			// to draw the whole mesh.
			AddSubMesh(_name.c_str(), iSize / sizeof(unsigned short), 0, 0);

			return disposer;
		}

	public:
		const std::string& Name() const { return _name; }

		const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView() const { assert(_uploadersDisposed); return &_vertexBufferView; }
		const D3D12_INDEX_BUFFER_VIEW*  IndexBufferView()  const { assert(_uploadersDisposed); return &_indexBufferView; }

		void DisposeUploaders() {
			_vertexUploadBuffer = nullptr;
			_indexUploadBuffer  = nullptr;

			_vertexBufferView.BufferLocation = _vertexBufferGPU->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes  = _vertexByteStride;
			_vertexBufferView.SizeInBytes    = _vertexBufferByteSize;

			_indexBufferView.BufferLocation = _indexBufferGPU->GetGPUVirtualAddress();
			_indexBufferView.Format         = _indexFormat;
			_indexBufferView.SizeInBytes    = _indexBufferByteSize;

			_uploadersDisposed = true;
		}

	public:

		void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const {
			pCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			pCommandList->IASetIndexBuffer(&_indexBufferView);
		}

		void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) {
			auto subMesh = _subMeshes[pMeshName];
			pCommandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}

	public:

		void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) {
			MCSubMesh subMesh;
			subMesh.IndexCount         = indexCount;
			subMesh.StartIndexLocation = startIndexLocation;
			subMesh.BaseVertexLocation = baseVertexLocation;
			_subMeshes[pMeshName] = subMesh;
		}

	private:
		ComPtr<ID3D12Resource> _vertexBufferGPU = nullptr;
		ComPtr<ID3D12Resource> _indexBufferGPU  = nullptr;

		ComPtr<ID3D12Resource> _vertexUploadBuffer = nullptr;
		ComPtr<ID3D12Resource> _indexUploadBuffer  = nullptr;
	private:
		std::string _name;
		UINT        _vertexByteStride;
		UINT        _vertexBufferByteSize;
		DXGI_FORMAT _indexFormat = DXGI_FORMAT_R16_UINT;
	    UINT        _indexBufferByteSize;

	private:
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW  _indexBufferView;

	private:
		std::unordered_map<std::string, MCSubMesh> _subMeshes;

	private:
		bool _uploadersDisposed = false;
	};

	class MCDynamicMesh {

	};

}