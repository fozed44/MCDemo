#pragma once

#include <thread>

#include "../../../../Common/MCCommon/src/Headers/MCManagedKeyedHandle.h"
#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCResult.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCResourceHandle {
		ID3D12Resource*  pResource;
		unsigned __int64 FenceValue;
		inline ID3D12Resource* Key() { return pResource; }
	};

	class MCResourceManager : public MCManagedKeyedHandleManager<MCResourceHandle, ID3D12Resource*, ComPtr<ID3D12Resource>, MCResourceManager> {
	public:
		MCResourceManager();
		MCResourceManager(MCResourceManager&)              = delete;
		MCResourceManager(MCResourceManager&&)             = delete;
		MCResourceManager& operator= (MCResourceManager&)  = delete;
		MCResourceManager& operator= (MCResourceManager&&) = delete;
		~MCResourceManager() {}
	public:
		static MCResourceManager* Instance() {
			return MCREGlobals::pResourceManager;
		}
	public:
		MC_RESULT GetResource(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource** ppResource);
		MC_RESULT GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource **pResource);
		ID3D12Resource *GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle);
		

		//MCResourceHandle CreateResource(MC_RESOURCE_MANAGER_RESOURCE_TYPE, size_t sizeInBytes);
		
		inline MCResourceManager::tManagedKeyedHandle CreateDefaultBufferTemp(void *pInitData, unsigned __int64 sizeInBytes)
		{
			return CreateDefaultBuffer(pInitData, sizeInBytes);
		}

	private:
		MCResourceHandle CreateConstantBuffer(size_t sizeInBytes);
		MCResourceManager::tManagedKeyedHandle CreateDefaultBuffer(void *pInitData, unsigned __int64 sizeInBytes);
	private:
		ComPtr<ID3D12CommandAllocator>     _pCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList>  _pCommandList;
		ComPtr<ID3D12Resource>             _pUploadBuffer;
		__int64 _lastFenceValue;
	};
}
