//#pragma once
//
//#include "MCD3D12Core.h"
//#include "MCD3D12RenderUtilities.h"
//#include <unordered_map>
//
//#include "MCUploadBuffer.h"
//#include "../Memory/MCResourceManager.h"
//
//using Microsoft::WRL::ComPtr;
//
//namespace MC {
//
//	template <typename TVERTEX_TYPE> class MCStaticMesh16;
//	template <typename TVERTEX_TYPE> class MCDynamicMesh16;
//
//	struct MCSubMesh {
//		UINT IndexCount         = 0;
//		UINT StartIndexLocation = 0;
//		UINT BaseVertexLocation = 0;		
//	};
//
//	/*
//		MCMesh is the base class for all meshes.
//	*/
//	class MCMesh {
//	public:
//		MCMesh(std::string &name) : _name(name) {}
//		MCMesh(const char *pName) : _name(pName) {}
//		virtual ~MCMesh() {}
//	public:
//		virtual void Draw(ID3D12GraphicsCommandList *pCommandList) = 0;
//		virtual void Draw(const std::string& meshName, ID3D12GraphicsCommandList *pCommandList) = 0;
//	public:
//		const std::string& Name() const { return _name; }
//	public:
//		void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) {
//			MCSubMesh subMesh;
//			subMesh.IndexCount         = indexCount;
//			subMesh.StartIndexLocation = startIndexLocation;
//			subMesh.BaseVertexLocation = baseVertexLocation;
//			_subMeshes[pMeshName] = subMesh;
//		}
//	private:
//		std::string _name;
//	protected:
//		std::unordered_map<std::string, MCSubMesh> _subMeshes;
//	};
//
//	template <typename TVERTEX_TYPE>
//	class MCStaticMesh16UploaderDisposer {
//	public:
//		MCStaticMesh16UploaderDisposer(MCStaticMesh16<TVERTEX_TYPE> *pMesh)
//			: _pMesh(pMesh) {}
//		MCStaticMesh16UploaderDisposer(MCStaticMesh16UploaderDisposer&)            = delete;
//		MCStaticMesh16UploaderDisposer& operator=(MCStaticMesh16UploaderDisposer&) = delete;
//		MCStaticMesh16UploaderDisposer(MCStaticMesh16UploaderDisposer&& o) {
//			this->_pMesh = o._pMesh;
//			o._pMesh = nullptr;
//		}
//		~MCStaticMesh16UploaderDisposer() {
//			if (_pMesh)
//				_pMesh->DisposeUploaders();
//		}
//	private:
//		MCStaticMesh16<TVERTEX_TYPE> *_pMesh;
//	};
//
//	/*template <typemane TVERTEX_TYPE>
//	class MCDynamicMesh16UploaderDisposer {
//	public:
//		MCDynamicMesh16UploaderDisposer(MCDynamicMesh16<TVERTEX_TYPE> *pMesh)
//			: _pMesh(pMesh) {}
//		MCDynamicMesh16UploaderDisposer(MCDynamicMesh16UploaderDisposer&) = delete;
//		MCDynamicMesh16UploaderDisposer& operator=(MCDynamicMesh16UploaderDisposer&) = delte;
//		MCDynamicMesh16UploaderDisposer(MCDynamicMesh16UploaderDisposer&& o) {
//			this->_pMesh = o._pMesh;
//			o._pMesh = nullptr;
//		}
//		~MCDynamicMesh16UploaderDisposer() {
//			if (_pMesh)
//				_pMesh->DisposeUploaders();
//		}
//	private:
//		MCDynamicMesh16<TVERTEX_TYPE> *_pMesh;
//	};*/
//
//	/*
//		MCStaticMesh16 is a static mesh, (D3D12_HEAP_TYPE_DEFAULT) with a vertex type of VERTEX_TYPE and an
//		index type of DXGI_FORMAT_R16_UINT
//	*/
//	template <typename TVERTEX_TYPE>
//	class MCStaticMesh16 {
//	public:
//		MCStaticMesh16(std::string& name) {
//			_name = name;
//		}
//		MCStaticMesh16(const char* pName) {
//			_name = std::string(pName);
//		}
//		MCStaticMesh16(MCStaticMesh16&)            = delete;
//		MCStaticMesh16& operator=(MCStaticMesh16&) = delete;
//		~MCStaticMesh16() {}
//
//		MCStaticMesh16UploaderDisposer<TVERTEX_TYPE> Upload(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, TVERTEX_TYPE *pVert, UINT vSize, unsigned short *pIndicies, UINT iSize) {
//			// For the time being, this Upload method does not attempt to check the current status of the mesh to see if
//			// data has already been loaded. Since this check is not being done, if Upload is called on a mesh that has already
//			// been loaded with data, the data that already exists at the time the method is called will not be properly destroyed.
//
//			// For this reason, we are not allowing Upload to be called more that once on a Mesh. We are going to ensure
//			// this by asserting that the _pVertexBufferGPU and _pIndexBufferGPU are null. 
//			assert(_pVertexBufferGPU == nullptr);
//			assert(_pIndexBufferGPU  == nullptr);
//
//			// Create the disposer that will be returned to the caller.
//			MCStaticMesh16UploaderDisposer<TVERTEX_TYPE> disposer(this);
//
//			// Store the meta information about this mesh.
//			_vertexByteStride     = sizeof(TVERTEX_TYPE);
//			_vertexBufferByteSize = vSize;
//			_indexBufferByteSize  = iSize;
//
//			// Create the vertex and index buffer.
//
//			// Note: 
//			//	Remember that CreateDefaultBuffer is an async operation. The method only adds the commands to copy the mesh
//			//  data into the GPU buffers to the command list. Also remember that the resulting GPU buffers are read only and
//			//  the vertex data must be copied to these buffers via the upload buffers (the output parameters passed to this method)
//			//  and these upload buffers must be kept alive until the command list has executed the copy (enter the disposer returned
//			//  by this method).
//			_pVertexBufferGPU      = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pVert,     vSize, _pVertexUploadBuffer);
//			_pIndexBufferGPU       = MCD3D12RenderUtilities::CreateDefaultBuffer(pDevice, pCommandList, pIndicies, iSize, _pIndexUploadBuffer);
//
//			// Set the name of the GPU buffers.
//			_pVertexBufferGPU->SetName(s2ws(_name + std::string(" gpu vertex buffer")).c_str());
//			_pIndexBufferGPU ->SetName(s2ws(_name + std::string(" gpu index buffer")).c_str());
//
//			// Set the name of the Upload buffers (even though) they are temporary.
//			_pVertexUploadBuffer->SetName(s2ws(_name + std::string(" vertex upload buffer")).c_str());
//			_pIndexUploadBuffer ->SetName(s2ws(_name + std::string(" index upload buffer")).c_str());
//
//			// Next we add the entire mesh as a subMesh, allowing the client to call DrawSubMesh using the name of this mesh
//			// to draw the whole mesh.
//			AddSubMesh(_name.c_str(), iSize / sizeof(unsigned short), 0, 0);
//
//			return disposer;
//		}
//
//	public:
//		const std::string& Name() const { return _name; }
//
//		const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView() const { assert(_uploadersDisposed); return &_vertexBufferView; }
//		const D3D12_INDEX_BUFFER_VIEW*  IndexBufferView()  const { assert(_uploadersDisposed); return &_indexBufferView; }
//
//		void DisposeUploaders() {
//			_pVertexUploadBuffer = nullptr;
//			_pIndexUploadBuffer  = nullptr;
//
//			_vertexBufferView.BufferLocation = _pVertexBufferGPU->GetGPUVirtualAddress();
//			_vertexBufferView.StrideInBytes  = _vertexByteStride;
//			_vertexBufferView.SizeInBytes    = _vertexBufferByteSize;
//
//			_indexBufferView.BufferLocation = _pIndexBufferGPU->GetGPUVirtualAddress();
//			_indexBufferView.Format         = _indexFormat;
//			_indexBufferView.SizeInBytes    = _indexBufferByteSize;
//		}
//
//	public:
//
//		void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const {
//			pCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
//			pCommandList->IASetIndexBuffer(&_indexBufferView);
//		}
//
//		void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) {
//			auto subMesh = _subMeshes[pMeshName];
//			pCommandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
//		}
//
//	public:
//
//		void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) {
//			MCSubMesh subMesh;
//			subMesh.IndexCount         = indexCount;
//			subMesh.StartIndexLocation = startIndexLocation;
//			subMesh.BaseVertexLocation = baseVertexLocation;
//			_subMeshes[pMeshName] = subMesh;
//		}
//
//	private:
//		ComPtr<ID3D12Resource> _pVertexBufferGPU = nullptr;
//		ComPtr<ID3D12Resource> _pIndexBufferGPU  = nullptr;
//
//		ComPtr<ID3D12Resource> _pVertexUploadBuffer = nullptr;
//		ComPtr<ID3D12Resource> _pIndexUploadBuffer  = nullptr;
//	private:
//		std::string _name;
//		UINT        _vertexByteStride;
//		UINT        _vertexBufferByteSize;
//		DXGI_FORMAT _indexFormat = DXGI_FORMAT_R16_UINT;
//	    UINT        _indexBufferByteSize;
//
//	private:
//		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
//		D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
//
//	private:
//		std::unordered_map<std::string, MCSubMesh> _subMeshes;
//	};
//
//	template <typename TVERTEX_TYPE>
//	class MCDynamicMesh16 {
//	public:
//		MCDynamicMesh16(std::string &name) {
//			_name = name;
//		}
//		MCDynamicMesh16(const char* pName) {
//			_name = pName;
//		}
//		MCDynamicMesh16(MCDynamicMesh16&) = delete;
//		MCDynamicMesh16& operator=(MCDynamicMesh16&) = delete;
//		~MCDynamicMesh16() {}
//
//		void Upload(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, TVERTEX_TYPE *pVert, UINT vSize, unsigned short *pIndicies, UINT iSize) {
//			assert(_pVertexBuffer == nullptr);
//			assert(_pIndexBuffer  == nullptr);
//
//			_pVertexBuffer = std::make_unique<MCUploadBuffer<TVERTEX_TYPE>>(pDevice, vSize / sizeof(TVERTEX_TYPE), false);
//			_pIndexBuffer  = std::make_unique<MCUploadBuffer<unsigned short>>(pDevice, iSize / sizeof(unsigned short), false);
//
//			_pVertexBuffer->CopyData(0, pVert, vSize / sizeof(TVERTEX_TYPE));
//			_pIndexBuffer->CopyData(0, pIndicies, iSize / sizeof(unsigned short));
//
//			_vertexBufferView.BufferLocation = _pVertexBuffer->Resource()->GetGPUVirtualAddress();
//			_vertexBufferView.StrideInBytes  = sizeof(TVERTEX_TYPE);
//			_vertexBufferView.SizeInBytes    = vSize;
//
//			_indexBufferView.BufferLocation = _pIndexBuffer->Resource()->GetGPUVirtualAddress();
//			_indexBufferView.Format         = DXGI_FORMAT_R16_UINT;
//			_indexBufferView.SizeInBytes    = iSize;
//
//			AddSubMesh(_name.c_str(), iSize / sizeof(unsigned short), 0, 0);
//		}
//
//	public:
//
//		void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const {
//			pCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
//			pCommandList->IASetIndexBuffer(&_indexBufferView);
//		}
//
//		void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) {
//			auto subMesh = _subMeshes[pMeshName];
//			pCommandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
//		}
//
//	public:
//
//		void AddSubMesh(const char *pMeshName, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation) {
//			MCSubMesh subMesh;
//			subMesh.IndexCount = indexCount;
//			subMesh.StartIndexLocation = startIndexLocation;
//			subMesh.BaseVertexLocation = baseVertexLocation;
//			_subMeshes[pMeshName] = subMesh;
//		}
//
//	private:
//		std::string _name;
//
//	private:
//		std::unordered_map<std::string, MCSubMesh> _subMeshes;	
//	private:
//		std::unique_ptr<MCUploadBuffer<TVERTEX_TYPE>>   _pVertexBuffer = nullptr;
//		std::unique_ptr<MCUploadBuffer<unsigned short>> _pIndexBuffer  = nullptr;
//
//	private:
//		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
//		D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
//	};
//
//	/*
//	MCStaticMesh16 is a static mesh, (D3D12_HEAP_TYPE_DEFAULT) with a vertex type of VERTEX_TYPE and an
//	index type of DXGI_FORMAT_R16_UINT
//	*/
//	template <typename TVERTEX_TYPE>
//	class MCStaticAllocatedMesh16 : public MCMesh {
//	public:
//		MCStaticAllocatedMesh16(std::string& name) : MCMesh(name) {}
//		MCStaticAllocatedMesh16(const char* pName) : MCMesh(pName) {}
//		MCStaticAllocatedMesh16(MCStaticAllocatedMesh16&) = delete;
//		MCStaticAllocatedMesh16& operator=(MCStaticAllocatedMesh16&) = delete;
//		~MCStaticAllocatedMesh16() {}
//	public:
//		void Upload(TVERTEX_TYPE *pVert, UINT vSize, unsigned short *pIndicies, UINT iSize) {
//			// Store the meta information about this mesh.
//			_vertexByteStride     = sizeof(TVERTEX_TYPE);
//			_vertexBufferByteSize = vSize;
//			_indexBufferByteSize  = iSize;
//
//			_hVertexBuffer = MCResourceManager::Instance()->CreateDefaultBufferTemp(pVert, vSize);
//			auto vertexBufferResource = MCResourceManager::Instance()->GetResourceSync(_hVertexBuffer);
//			vertexBufferResource->SetName(s2ws(Name() + std::string(" gpu vertex buffer")).c_str());
//
//			_hIndexBuffer = MCResourceManager::Instance()->CreateDefaultBufferTemp(pIndicies, iSize);
//			auto indexBufferResource = MCResourceManager::Instance()->GetResourceSync(_hIndexBuffer);
//			indexBufferResource->SetName(s2ws(Name() + std::string(" gpu index buffer")).c_str());
//
//			// Next we add the entire mesh as a subMesh, allowing the client to call DrawSubMesh using the name of this mesh
//			// to draw the whole mesh.
//			AddSubMesh(Name().c_str(), iSize / sizeof(unsigned short), 0, 0);
//
//
//			_vertexBufferView.BufferLocation = vertexBufferResource->GetGPUVirtualAddress();
//			_vertexBufferView.StrideInBytes  = _vertexByteStride;
//			_vertexBufferView.SizeInBytes    = _vertexBufferByteSize;
//
//			_indexBufferView.BufferLocation = indexBufferResource->GetGPUVirtualAddress();
//			_indexBufferView.Format         = _indexFormat;
//			_indexBufferView.SizeInBytes    = _indexBufferByteSize;
//		}
//
//	public:
//
//		const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView() const { assert(_uploadersDisposed); return &_vertexBufferView; }
//		const D3D12_INDEX_BUFFER_VIEW*  IndexBufferView()  const { assert(_uploadersDisposed); return &_indexBufferView; }
//
//	public:
//
//		void SetIABuffers(ID3D12GraphicsCommandList *pCommandList) const {
//			pCommandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
//			pCommandList->IASetIndexBuffer(&_indexBufferView);
//		}
//
//		void DrawSubMesh(const char *pMeshName, ID3D12GraphicsCommandList *pCommandList) {
//			auto subMesh = _subMeshes[pMeshName];
//			pCommandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
//		}
//
//	public:
//
//		void Draw(ID3D12GraphicsCommandList *pCommandList) {
//			Draw(_subMeshes.begin()->first, pCommandList);
//		}
//
//		void Draw(const std::string& meshName, ID3D12GraphicsCommandList *pCommandList) {
//			SetIABuffers(pCommandList);
//			DrawSubMesh(meshName.c_str(), pCommandList);
//		}
//
//	private:
//		MCResourceManager::HandleType _hVertexBuffer;
//		MCResourceManager::HandleType _hIndexBuffer;
//
//	private:
//		UINT        _vertexByteStride;
//		UINT        _vertexBufferByteSize;
//		DXGI_FORMAT _indexFormat = DXGI_FORMAT_R16_UINT;
//		UINT        _indexBufferByteSize;
//
//	private:
//		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
//		D3D12_INDEX_BUFFER_VIEW  _indexBufferView;
//	};
//
//}