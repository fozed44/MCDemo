#include "MCResourceManager.h"
#include "../Core/MCD3D.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../../../../Common/MCCommon/src/Global/MCCOGlobals.h"

#include "MCResourceManagerAnalyzer.h"

namespace MC {

#pragma region ctor

	MCResourceManager::MCResourceManager() 
		: _uploadBufferFence{ 0 } {
		assert(MCREGlobals::pMCDXGI);

		// Create a new allocator
		MCThrowIfFailed(
			MCREGlobals::pMCDXGI->Get3DDevice()->
				CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					__uuidof(_pCommandAllocator),
					&_pCommandAllocator
				)
		);

		// Create a new command list.
		MCThrowIfFailed(
			MCREGlobals::pMCDXGI->Get3DDevice()->
			CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				_pCommandAllocator.Get(),
				nullptr,
				__uuidof(_pCommandList),
				&_pCommandList
			)
		);

		MCThrowIfFailed(_pCommandList->Close());

#ifdef _DEBUG
		_pAnalyzer = std::make_unique<MCResourceManagerAnalyzer>(this);
#endif _DEBUG
	}

	MCResourceManager::~MCResourceManager() {}

#pragma endregion
	
#pragma region static buffer

	MC_RESULT MCResourceManager::CreateStaticBufferAsync(void *pInitData, size_t numBytes, HResource* pResult) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		// At this point we need to make sure that the GPU is done using the upload buffer
		// from the last call to CreateStaticBufferAsync.
		if (MCREGlobals::pMCD3D->GetCurrentFenceValue() < _uploadBufferFence)
			return MC_RESULT::FAIL_UPLOADING;

		// the result.
		ComPtr<ID3D12Resource> defaultBuffer;

		MCThrowIfFailed(_pCommandAllocator->Reset());
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(numBytes),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			__uuidof(defaultBuffer),
			&defaultBuffer
		));

		// In order to copy CPU memory data (initData) into a GPU default buffer (defaultBuffer) an intermediate upload
		// buffer must be created.
		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(numBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(_pUploadBuffer),
			&_pUploadBuffer
		));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData      = pInitData;
		subResourceData.RowPitch   = static_cast<LONG_PTR>(numBytes);
		subResourceData.SlicePitch = subResourceData.RowPitch;

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
		);

		UpdateSubresources<1>(
			_pCommandList.Get(),
			defaultBuffer.Get(),
			_pUploadBuffer.Get(),
			0,
			0,
			1,
			&subResourceData
		);

		_pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_GENERIC_READ
			)
		);

		MCThrowIfFailed(_pCommandList->Close());

		MCREGlobals::pMCD3D->ExecuteCommandList(_pCommandList.Get());
		
		// _uploadBufferFence will be used to prevent this method from using
		// the upload buffer before a previous call to this method has completed
		// its use of the upload buffer.
		_uploadBufferFence = MCREGlobals::pMCD3D->GetNewFenceValue();

		MCResourceLocal local{
			defaultBuffer.Get(),
			_uploadBufferFence
		};

		MCResourceDescriptor desc {
			MCRESOURCE_DESCRIPTOR_TYPE::STATIC_BUFFER, // Descriptor type
			std::move(defaultBuffer),                  // pResource
			_uploadBufferFence,                        // FenceValue
			nullptr                                    // MappedData
		};

		*pResult = std::move(this->CreateNewItem(
			desc,
			local
		));

		return MC_RESULT::OK;
	}

	HResource MCResourceManager::CreateStaticBufferSync(void *pInitData, size_t numBytes) {
		HResource hResource;
		while (CreateStaticBufferAsync(pInitData, numBytes, &hResource) != MC_RESULT::OK) {}

		MCREGlobals::pMCD3D->WaitForFenceValue(hResource._localData.FenceValue);

		// Since we have already waited for the resource to load, set its fence value to 0.
		hResource._localData.FenceValue = 0;

		return std::move(hResource);
	}

#pragma endregion

#pragma region dynamic constant buffer

	HResource MCResourceManager::CreateDynamicConstantBuffer(void *pInitData, size_t numBytes) {
		MCTHREAD_ASSERT(MC_THREAD_CLASS::MAIN);

		// AlignSize aligns the buffer size to hardware dependent size.
		auto alignedSize = CalculateConstantBufferSize(numBytes);

		// The descriptor for the buffer we are going to be creating.
		MCResourceDescriptor desc;
		desc.DescriptorType = MCRESOURCE_DESCRIPTOR_TYPE::DYNAMIC_CONSTANT_BUFFER;

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(alignedSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(desc.pResource),
			&desc.pResource
		));

		MCThrowIfFailed(desc.pResource->Map(0, nullptr, reinterpret_cast<void**>(&desc.pMappedData)));

		if(pInitData)
			memcpy(desc.pMappedData, pInitData, numBytes);

		return std::move(this->CreateNewItem(
			desc,
			{
				desc.pResource.Get(),
				reinterpret_cast<unsigned __int64>(desc.pMappedData)
			}
		));
	}

	MC_RESULT MCResourceManager::MapDynamicConstantData(const HResource& hResource, void *pData, size_t numBytes) {
		memcpy(hResource._localData.pMappedData, pData, numBytes);
		return MC_RESULT::OK;
	}

#pragma endregion

#pragma region Get Resource

	MC_RESULT MCResourceManager::GetStaticResourceAsync(const HResource& hResource, ID3D12Resource **ppResource) const {
		auto fenceValueAlias = hResource._localData.FenceValue;

		if (fenceValueAlias) {
			if (MCREGlobals::pMCD3D->GetCurrentFenceValue() < fenceValueAlias)
				return MC_RESULT::FAIL_UPLOADING;

			// Once we know the fence value has been reached, we set FenceValue to zero. This allows
			// us to check the loaded status of the resource via if(FenceValue) rather than having
			// to compare an actual fence value to GetCurrentFenceValue();
			const_cast<HResource&>(hResource)._localData.FenceValue = 0;
		}
		*ppResource = hResource._localData.pResource;
		return MC_RESULT::OK;
	}

	ID3D12Resource *MCResourceManager::GetStaticResourceSync(const HResource& hResource) const {
		auto fenceValueAlias = hResource._localData.FenceValue;

		if (fenceValueAlias) {
			MCREGlobals::pMCD3D->WaitForFenceValue(fenceValueAlias);

			// Once we know the fence value has been reached, we set FenceValue to zero. This allows
			// us to check the loaded status of the resource via if(FenceValue) rather than having
			// to compare an actual fence value to GetCurrentFenceValue();
			const_cast<HResource&>(hResource)._localData.FenceValue = 0;
		}

		return hResource._localData.pResource;
	}

	ID3D12Resource *MCResourceManager::GetDynamicResource(const HResource &hResource) {
		return hResource._localData.pResource;
	}

#pragma endregion

#pragma region debug name

#ifdef _DEBUG
	void MCResourceManager::SetDbgName(HResource& hResource, const char* dbgName) {
		auto pDescriptor = GetManagedItem(hResource);
		pDescriptor->dbgName = dbgName;
	}
#endif _DEBUG

#pragma endregion

	size_t MCResourceManager::CalculateConstantBufferSize(size_t size) const {
		/*
		Constant buffers must be a multiple of the minimum hardware allocation size (usually 256 bytes),
		So round up to the nearest multiple of 256. We do this by adding 255 and then masking off the lower
		byte which stores all bits < 256

		Example: Suppose a byte size of 300:
		(300 + 255) & ~0x000000ff
		(555) & 0xFFFFFF00
		0x0000022B & 0xFFFFFF00 =
		0x00000200 = 512

		*/
		return (size + 255) & ~255;
	}
}
