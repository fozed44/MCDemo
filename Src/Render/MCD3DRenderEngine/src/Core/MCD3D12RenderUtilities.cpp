#include "MCD3D12RenderUtilities.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

using Microsoft::WRL::ComPtr;

namespace MC {
namespace MCD3D12RenderUtilities {

	/*
	Note:
	'uploadBuffer' has to be kept alive until AFTER the command list is executed.
	*/
	ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer) {

		// the result.
		ComPtr<ID3D12Resource> defaultBuffer;

		MCThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			__uuidof(defaultBuffer),
			&defaultBuffer
		));

		// In order to copy CPU memory data (initData) into a GPU default buffer (defaultBuffer) an intermediate upload
		// buffer must be created.
		MCThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(uploadBuffer),
			&uploadBuffer
		));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
		);
		UpdateSubresources<1>(
			pCommandList,
			defaultBuffer.Get(),
			uploadBuffer.Get(),
			0,
			0,
			1,
			&subResourceData
			);
		pCommandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_GENERIC_READ
			)
		);

		return defaultBuffer;
	}

	void OrderedStaticDestroy() {
		MCD3D::Instance()->OrderedStaticDestroy();
	}

}}