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
		inline ID3D12Resource* Key() const noexcept { return pResource; }
	};

	class MCResourceManager : public MCManagedKeyedHandleManager<MCResourceHandle, ID3D12Resource*, ComPtr<ID3D12Resource>, MCResourceManager> {
	public:
		using HResource = MCResourceManager::HandleType;
	public:
		MCResourceManager();
		MCResourceManager(MCResourceManager&)              = delete;
		MCResourceManager(MCResourceManager&&)             = delete;
		MCResourceManager& operator= (MCResourceManager&)  = delete;
		MCResourceManager& operator= (MCResourceManager&&) = delete;
		~MCResourceManager() {}
	public:
		/*
			The Instance method is required because the MCManagedKeyedHandleManager base class requires it.
		*/
		static MCResourceManager* Instance() {
			return MCREGlobals::pResourceManager;
		}
	public:
		inline MC_RESULT GetResource(const MCResourceManager::HandleType& handle, ID3D12Resource** ppResource) const;
		inline MC_RESULT GetResourceSync(const MCResourceManager::HandleType& handle, ID3D12Resource **pResource) const;
		inline ID3D12Resource *GetResourceSync(const MCResourceManager::HandleType& handle) const;
		

		//MCResourceHandle CreateResource(MC_RESOURCE_MANAGER_RESOURCE_TYPE, size_t sizeInBytes);
		
		inline MC_RESULT CreateStaticBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateStaticBufferSync (void *pInitData, size_t numBytes, bool syncLoad);

	private:
		HResource CreateConstantBuffer(size_t sizeInBytes);
		HResource CreateDefaultBuffer(void *pInitData, unsigned __int64 sizeInBytes);
	private:
		ComPtr<ID3D12CommandAllocator>     _pCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList>  _pCommandList;

		ComPtr<ID3D12Resource>             _pUploadBuffer;
		unsigned __int64                   _uploadBufferFence; // Used to prevent the upload buffer from
		                                                       // being double accessed from two consecutive
															   // CreateBuffer calls.
	};
}
