#pragma once

#include <thread>

#include "../../../../Common/MCCommon/src/Headers/MCManagedLinearBufferHandle.h"
#include "../Core/MCD3D12Core.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCResult.h"

using Microsoft::WRL::ComPtr;

constexpr unsigned int MCRESOURCE_MANAGER_BUFFER_SIZE = 1000;

namespace MC {

	class MCResourceManagerAnalyzer;

	enum class MCRESOURCE_DESCRIPTOR_TYPE {
		INVALID                 = 0,
		STATIC_BUFFER           = 1,
		STATIC_CONSTANT_BUFFER  = 2,
		DYNAMIC_BUFFER          = 3,
		DYNAMIC_CONSTANT_BUFFER = 4
	};

	struct MCResourceDescriptor {
		MCRESOURCE_DESCRIPTOR_TYPE DescriptorType;
		ComPtr<ID3D12Resource>     pResource;
		unsigned __int64           FenceValue;   // Only used by static resources
		void*                      pMappedData;  // Only used by dynamic resources
#ifdef _DEBUG
		std::string                dbgName;      // dbgName is only available in debug builds
												 // use the SET_MCRESOURCE_DBG_NAME macro to
		                                         // set the resource name only in debug builds.
#endif _DEBUG
	};

	// As noted above, MCResourceDescriptor.dbgName is only available for debug builds, as is the
	// MCResourceManager.SetDbgName function. Use the SET_MCRESOURCE_DBG_NAME macro to set the debug 
	// name in debug builds and do nothing in release builds.
#ifdef _DEBUG
#define SET_MCRESUORCE_DBG_NAME(hResource, name) ::MC::MCResourceManager.SetDbgName((hResource), (name))
#else
#define SET_MCRESOURCE_DBG_NAME(hResource, name)
#endif _DEBUG

	struct MCResourceLocal {
		ID3D12Resource*  pResource;
		union {
			unsigned __int64 FenceValue; // The fence value is required for static resources only
			void           *pMappedData; // The mapped Data is required for dynamic resources only.
		};
	};


	class MCResourceManager : public MCManagedLinearBufferHandleManager<MCResourceDescriptor, MCResourceLocal, MCResourceManager, MCRESOURCE_MANAGER_BUFFER_SIZE> {
	public:
		using HResource = MCResourceManager::HandleType;
	public:
		MCResourceManager();
		MCResourceManager(MCResourceManager&)              = delete;
		MCResourceManager(MCResourceManager&&)             = delete;
		MCResourceManager& operator= (MCResourceManager&)  = delete;
		MCResourceManager& operator= (MCResourceManager&&) = delete;
		~MCResourceManager();
	public:
		/* The Instance method is required because the MCManagedKeyedHandleManager base class requires it. */
		static MCResourceManager* Instance() { return MCREGlobals::pResourceManager; }
	public:
		/* GetStaticResourceAsync attempts to get the static resource pointed to by the given handle. The method can fail
		   if the static resource has not completed loading. */
		MC_RESULT GetStaticResourceAsync(const HResource& hResource, ID3D12Resource **ppResource) const;

		/* GetStaticResourceSync attempts to get the static resource pointed to by the given handle. The method call
		   will block if the static resource has not been loaded. */
		ID3D12Resource *GetStaticResourceSync(const HResource& hResource) const;
		
		/* GetDynamicResource returns the dynamic resource pointed to by the given handle. Dynamic resources
		   do not require time to upload like static resources do. */
		ID3D12Resource *GetDynamicResource(const HResource& hResource);

		/* Create a static buffer. 
			The async version can return MC_RESULT::FAIL_UPLOADING if the upload resource is unavailable.
			Further, the async version will return as soon as the resource is queued to upload, it will not
			wait for the initial data to be uploaded. 
			
			The sync version will block until the upload buffer is available AND the resource has been
			uploaded to the card. */
		MC_RESULT CreateStaticBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateStaticBufferSync (void *pInitData, size_t numBytes);

		/*MC_RESULT CreateDynamicBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateDynamicBufferSync (void *pInitData, size_t numBytes, bool       syncLoad); */

		/* Create a dynamic constant buffer.
			The async version can return MC_RESULT::FAIL_UPLOADING if the upload resource is unavailable.
			Further, the async version will return as soon as the resource is queued to upload, it will not
			wait for the initial data to be uploaded.

			The sync version will block until the upload buffer is available AND the resource has been
			uploaded to the card. */
		HResource CreateDynamicConstantBuffer(void *pInitData, size_t numBytes);
		MC_RESULT MapDynamicConstantData(const HResource& hResource, void *pData, size_t numBytes);

		/*MC_RESULT CreateStaticConstantBufferAsync(void *pInitData, size_t numBytes, HResource* pResult);
		HResource CreateStaticConstantBufferSync (void *pInitData, size_t numBytes, bool       syncLoad);

		MC_RESULT MapDynamicByfferAsync(HResource& handle, void *pData, size_t numBytes);
		void      MapDynamicByfferSync (HResource& handle, void *pData, size_t numBytes);	*/	
				
#ifdef _DEBUG
		/* Do not access this method directly, as it is only available in debug mode. Instead, use the
		   SET_MCRESOURCE_DBG_NAME to make sure the statement to set the debug name does nothing in
		   release builds. */
		void SetDbgName(HResource& hResource, const char* dbgName);
#endif _DEBUG

	private:
		MCResourceDescriptor* GetDescriptor(const HResource& hResource);

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

		/* The ResourceAnalyzer has direct access to private data, even though it should always be
		   accessing the data in a read only fashion. */
		/* The analyzer will be created in the constructor and will attach itself to the router in
		   order to begin listening for analysis messages. */
#ifdef _DEBUG
		friend MCResourceManagerAnalyzer;
		std::unique_ptr<MCResourceManagerAnalyzer> _pAnalyzer;
	public:
		MCResourceManagerAnalyzer* GetAnalyzer() { return _pAnalyzer.get(); }
#endif _DEBUG
	};

	using HResource = MCResourceManager::HResource;
}
