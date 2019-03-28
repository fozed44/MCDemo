#include "MCResourceManager.h"
#include "../Core/MCD3D.h"
#include "../Core/MCREGlobals.h"

namespace MC {

	MCResourceManager::MCResourceManager() {
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

	/*
	Note:
	'uploadBuffer' has to be kept alive until AFTER the command list is executed.
	*/
	MCResourceManager::tManagedKeyedHandle MCResourceManager::CreateDefaultBuffer(void *pInitData, unsigned __int64 sizeInBytes) {

		// the result.
		ComPtr<ID3D12Resource> defaultBuffer;

		MCThrowIfFailed(_pCommandAllocator->Reset());
		MCThrowIfFailed(_pCommandList->Reset(_pCommandAllocator.Get(), nullptr));

		MCThrowIfFailed(MCREGlobals::pMCDXGI->Get3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes),
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
			&CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(_pUploadBuffer),
			&_pUploadBuffer
		));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData      = pInitData;
		subResourceData.RowPitch   = sizeInBytes;
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

		auto nextFence = MCREGlobals::pMCD3D->GetNewFenceValue();

		MCResourceHandle newHandle{ defaultBuffer.Get(), nextFence };

		return this->CreateRef(newHandle, defaultBuffer);
	}

	MC_RESULT MCResourceManager::GetResource(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource **ppResource) {
		if (MCREGlobals::pMCD3D->GetCurrentFenceValue() < handle.Handle().FenceValue)
			return MC_RESULT_FAIL_UPLOADING;
		*ppResource = handle.Handle().pResource;
		return MC_RESULT_OK;
	}
	MC_RESULT MCResourceManager::GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle, ID3D12Resource **ppResource) {
		*ppResource = GetResourceSync(handle);
		return MC_RESULT_OK;
	}

	ID3D12Resource *MCResourceManager::GetResourceSync(const MCResourceManager::tManagedKeyedHandle& handle) {
		MCREGlobals::pMCD3D->WaitForFenceValue(handle.Handle().FenceValue);
		return handle.Handle().pResource;
	}
}
