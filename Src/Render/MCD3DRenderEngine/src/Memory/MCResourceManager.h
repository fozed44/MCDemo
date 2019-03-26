#pragma once

#include <thread>

#include "../../../../Common/MCCommon/src/Headers/MCManagedKeyedHandle.h"
#include "../Core/MCD3D12Core.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	struct MCResourceHandle {
		ID3D12Resource*  pResource;
		unsigned __int64 FenceValue;
		inline ID3D12Resource* Key() { return pResource; }
	};

	enum MC_RESOURCE_MANAGER_ERROR {
		MC_RESOURCE_MANAGER_ERROR_OK,
		MC_RESOURCE_MANAGER_ERROR_RESOURCE_LOST,
		MC_RESOURCE_MANAGER_ERROR_UPLOADING
	};

	enum MC_RESOURCE_MANAGER_RESOURCE_TYPE {
		MC_RESOURCE_MANAGER_RESOURCE_TYPE_CONSTANT,
		MC_RESOURCE_MANAGER_RESOURCE_TYPE_DYNAMIC
	};

	class MCResourceManager : public MCManagedKeyedHandleManager<MCResourceHandle, ID3D12Resource*, ComPtr<ID3D12Resource>, MCResourceManager> {
	public:
		MCResourceManager(MCResourceManager&)              = delete;
		MCResourceManager(MCResourceManager&&)             = delete;
		MCResourceManager& operator= (MCResourceManager&)  = delete;
		MCResourceManager& operator= (MCResourceManager&&) = delete;
		~MCResourceManager() {}
	public:
		static MCResourceManager* Instance() {
			static MCResourceManager instance;
			return &instance;
		}
	public:
		/*
			Initialize the resource manager. This method must be called before any other resource manager method. It also
			must be called AFTER MCDXGI and MCD3D have been initialized.
		*/
		void Initialize();

		MC_RESOURCE_MANAGER_ERROR GetResource(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource** ppResource);
		MC_RESOURCE_MANAGER_ERROR GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource **pResource);
		ID3D12Resource *GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle);
		

		MCResourceHandle CreateResource(MC_RESOURCE_MANAGER_RESOURCE_TYPE, size_t sizeInBytes);
		
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
		MCResourceManager() {}
	};
}
