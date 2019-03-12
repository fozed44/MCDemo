#pragma once

#include "MCD3D12Core.h"
#include "MCD3D12RenderUtilities.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	template <typename TVERTEX_TYPE> class MCStaticMesh16;
	template <typename TVERTEX_TYPE> class MCDynamicMesh16;

	template <typename TVERTEX_TYPE>
	class MCStaticMeshUploaderDisposer {
		MCStaticMeshUploaderDisposer(MCStaticMesh16<TVERTES_TYPE> *pMesh)
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
		MCStaticMesh16<TVERTEX_TYPE> _pMesh;
	};

	/*
		MCStaticMesh16 is a static mesh, (D3D12_HEAP_TYPE_DEFAULT) with a vertex type of VERTEX_TYPE and an
		index type of DXGI_FORMAT_R16_UINT
	*/
	template <typename TVERTEX_TYPE>
	class MCStaticMesh16 {
	public:
		MCStaticMesh16(std::string& name, TVERTEX_TYPE *pVert, size_t vSize, unsigned short *pIndicies, size_t iSize) {
			_name                 = name;
			_vertexByteStride     = sizeof(TVERTEX_TYPE);
			_vertexBufferByteSize = vSize;
			_indexBufferByteSize  = iSize;
		}
		MCStaticMesh16(MCStaticMesh16&)            = delete;
		MCStaticMesh16& operator=(MCStaticMesh16&) = delete;
		~MCStaticMesh16() {}

		MCStaticMeshUploaderDisposer upload(ID3D12Device *pDevice, ID3DGraphicsCommandList *pCommandList) {
			MCStaticMeshUploaderDisposer<TVERTEX_TYPE> disposer;
			_vertexBufferGPU = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pVert, vSize, _vertexUploadBuffer);
			_indexBufferGPU = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pIndicies, iSize, _indexUploadBuffer);

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

	private:
		ComPtr<ID3DBlob> _vertexBufferCPU = nullptr;
		ComPtr<ID3DBlob> _indexBufferCPU  = nullptr;

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
		bool _uploadersDisposed = false;
	};

	class MCDynamicMesh {

	};

}