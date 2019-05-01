#include "MCResourceManager.h"
#include "../Core/MCD3D.h"
#include "../Core/MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"

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
	}

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

		MCResourceHandle handle{
			defaultBuffer.Get(),
			_uploadBufferFence // Reuse the fence value loaded for the upload buffer as the
							   // fence value that will be used to let us know when the static
							   // buffer is ready.
		};


		*pResult = std::move(this->CreateRef(
			handle,
			defaultBuffer
		));

		return MC_RESULT::OK;
	}

	MCResourceManager::HandleType MCResourceManager::CreateStaticBufferSync(void *pInitData, size_t numBytes, bool syncLoad) {
		HResource hResource;
		while (CreateStaticBufferAsync(pInitData, numBytes, &hResource) != MC_RESULT::OK) {}

		auto unwrappedHandle = UnwrapHandle(hResource);
		MCREGlobals::pMCD3D->WaitForFenceValue(unwrappedHandle.FenceValue);

		// Since we have already waited for the resource to load, set its fence value to 0.
		unwrappedHandle.FenceValue = 0;

		return std::move(hResource);
	}

#pragma endregion

	MC_RESULT MCResourceManager::GetResource(const HResource& handle, ID3D12Resource **ppResource) const {
		auto unwrappedHandle = UnwrapHandle(handle);

		if (unwrappedHandle.FenceValue) {
			if (MCREGlobals::pMCD3D->GetCurrentFenceValue() < unwrappedHandle.FenceValue)
				return MC_RESULT::FAIL_UPLOADING;

			// Once we know the fence value has been reached, we set FenceValue to zero. This allows
			// us to check the loaded status of the resource via if(FenceValue) rather than having
			// to compare an actual fence value to GetCurrentFenceValue();
			const_cast<MCResourceHandle&>(unwrappedHandle).FenceValue = 0;
		}
		*ppResource = unwrappedHandle.pResource;
		return MC_RESULT::OK;
	}
	MC_RESULT MCResourceManager::GetResourceSync(MCResourceManager::HandleType& handle, ID3D12Resource **ppResource){
		*ppResource = GetResourceSync(handle);
		return MC_RESULT::OK;
	}

	ID3D12Resource *MCResourceManager::GetResourceSync(MCResourceManager::HandleType& handle){
		auto unwrappedHandle = UnwrapHandle(handle);

		if (unwrappedHandle.FenceValue) {
			MCREGlobals::pMCD3D->WaitForFenceValue(unwrappedHandle.FenceValue);

			// Once we know the fence value has been reached, we set FenceValue to zero. This allows
			// us to check the loaded status of the resource via if(FenceValue) rather than having
			// to compare an actual fence value to GetCurrentFenceValue();
			const_cast<MCResourceHandle&>(unwrappedHandle).FenceValue = 0;
		}

		return unwrappedHandle.pResource;
	}
}
