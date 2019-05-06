#pragma once

#include <thread>

#include "../../../../Common/MCCommon/src/Headers/MCManagedLinearBufferHandle.h"
#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCResult.h"

using Microsoft::WRL::ComPtr;

constexpr unsigned int MCRESOURCE_MANAGER_BUFFER_SIZE = 1000;

namespace MC {

	enum class MCRESOURCE_DESCRIPTOR_TYPE {
		INVALID                 = 0,
		STATIC_BUFFER           = 1,
		DYNAMIC_BUFFER          = 2,
		STATIC_CONSTANT_BUFFER  = 3,
		DYNAMIC_CONSTANT_BUFFER = 4
	};

	struct MCResourceDescriptor {
		MCRESOURCE_DESCRIPTOR_TYPE DescriptorType;
		ComPtr<ID3D12Resource>     pResource;
		unsigned __int64           FenceValue;
		void*                      MappedData;  // Only used by dynamic resources
	};


	class MCResourceManager : public MCManagedLinearBufferHandleManager<MCResourceDescriptor, MCRESOURCE_MANAGER_BUFFER_SIZE> {
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
		/* The Instance method is required because the MCManagedKeyedHandleManager base class requires it. */
		static MCResourceManager* Instance() { return MCREGlobals::pResourceManager; }
	public:
		/* GetResource attempts to get the resource pointed to by the given handle. The method can fail
		   if the resource has not completed loading. */
		MC_RESULT GetResource(const HResource& handle, ID3D12Resource **ppResource) const;
		MC_RESULT GetResource(      HResource& handle, ID3D12Resource **pResource);

		/* GetResourceSync attempts to et the resource pointed to by the given handle. The method call
		   will block if the resource has not been loaded. */
		MC_RESULT       GetResourceSync(MCResourceManager::HandleType& handle, ID3D12Resource **pResource);
		ID3D12Resource *GetResourceSync(MCResourceManager::HandleType& handle);
		

		MC_RESULT CreateStaticBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateStaticBufferSync (void *pInitData, size_t numBytes, bool       syncLoad);

		/*MC_RESULT CreateDynamicBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateDynamicBufferSync (void *pInitData, size_t numBytes, bool       syncLoad);

		MC_RESULT CreateDynamicConstantBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateDynamicConstantBufferSync (void *pInitData, size_t numBytes, bool       syncLoad);

		MC_RESULT CreateStaticConstantBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateStaticConstantBufferSync (void *pInitData, size_t numBytes, bool       syncLoad);

		MC_RESULT MapDynamicByfferAsync(HResource& handle, void *pData, size_t numBytes);
		void      MapDynamicByfferSync (HResource& handle, void *pData, size_t numBytes);	*/	

	private:
		size_t CalculateConstantBufferSize(size_t size) const;

		HResource CreateConstantBuffer(size_t sizeInBytes);
		HResource CreateDefaultBuffer (void *pInitData, unsigned __int64 sizeInBytes);
	private:
		ComPtr<ID3D12CommandAllocator>     _pCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList>  _pCommandList;

		ComPtr<ID3D12Resource>             _pUploadBuffer;
		unsigned __int64                   _uploadBufferFence; // Used to prevent the upload buffer from
		                                                       // being double accessed from two consecutive
															   // CreateBuffer calls.
	};

	using HResource = MCResourceManager::HResource;
}
