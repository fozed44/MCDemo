#pragma once

#include "MCD3D12Core.h"

using Microsoft::WRL::ComPtr;

namespace MC {
namespace MCD3D12RenderUtilities {


	/*
		CreateDefaultBuffer creates a D3D12_HEAP_TYPE_DEFAULT ID3D12Resource using CreateComittedResource. These
		types of resources are read only from the CPU. Since D3D12_HEAP_TYPE_DEFAULT can't be written to by the CPU,
		CreateDefaultBuffer first creates an UploadBuffer D3D12_HEAP_TYPE_UPLOAD, copies initData into the upload buffer,
		and then copies the upload buffer into the default buffer on the GPU side.

		Make sure to observe the note below about keeping UploadBuffer alive until after the data has been copied (the data
		is copied after the command buffer has been executed).

		Note:
			'uploadBuffer' has to be kept alive until AFTER the command list is executed.
	*/
	ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);

}}
