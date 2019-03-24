//#pragma once
//
//#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"
//#include "../Core/MCD3D12Core.h"
//
//using Microsoft::WRL::ComPtr;
//
//namespace MC {
//
//	struct MCResourceHandle {
//		ID3D12Resource* pResource;
//		__int64         FenceValue;
//		operator bool() const {
//			return pResource == 0;
//		}
//	};
//
//	class MCResourceManager;
//
//	typedef MCManagedHandle <MCResourceHandle, MCResourceManager> MCResourceHandle;
//
//	class MCResourceManager : MCManagedHandleManager<ID3D12Resource*, ComPtr<ID3D12Resource>, MCResourceManager> {
//	public:
//		MCResourceManager(MCResourceManager&) = delete;
//		MCResourceManager(MCResourceManager&&) = delete;
//		MCResourceManager& operator= (MCResourceManager&) = delete;
//		MCResourceManager& operator= (MCResourceManager&&) = delete;
//		~MCResourceManager() {}
//	public:
//		MCResourceManager * Instance() {
//			static MCResourceManager instance;
//			return &instance;
//		}
//
//	public:
//
//		MCResourceHandle CreateDefaultResource<tBuffer>(tBuffer *pBuffer, size_t numElements) {
//
//		}
//
//		MCResourceHandle CreateDefaultResource(char *pBUffer, size_t bufferSize) {
//
//		}
//
//		template<typename tBuffer>
//		MCResourceHandle LoadConstantBuffer<tBuffer>(tBuffer* pBuffer, size_t numElements) {
//			return LoadConstantBuffer(pBuffer, sizeof(tBuffer), size);
//		}
//		MCResourceHandle LoadDDSTexture(const char *pFilename);
//
//	private:
//		MCResourceHandle LoadConstantBuffer(void *pBuffer, UINT elementSize, size_t size);
//	private:
//		MCResourceManager() {}
//	};
//}
