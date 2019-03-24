#pragma once

#include "MCD3D12Core.h"
#include "MCD3D.h"

using Microsoft::WRL::ComPtr;

namespace MC {
namespace MCD3D12RenderUtilities {


	/*
		CreateDefaultBuffer creates a D3D12_HEAP_TYPE_DEFAULT ID3D12Resource using CreateComittedResource. These
		types of resources are r inaccessible to the CPU. Since D3D12_HEAP_TYPE_DEFAULT can't be written to by the CPU,
		CreateDefaultBuffer first creates an UploadBuffer D3D12_HEAP_TYPE_UPLOAD, copies initData into the upload buffer,
		and then copies the upload buffer into the default buffer on the GPU side.

		Make sure to observe the note below about keeping UploadBuffer alive until after the data has been copied (the data
		is copied after the command buffer has been executed).

		Note:
			'uploadBuffer' has to be kept alive until AFTER the command list is executed.
	*/
	ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device *pDevice, ID3D12GraphicsCommandList *pCommandList, void *initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);

	/*
		The OrderedStatisDestroy is a 'Shutdown method' that allows data within the various singletons to be destroyed in
		a desired order.

		The render engine and its framework contains many singleton classes. Many of these classes have Initialize methods
		that allow the framework to be initialized in a specific order. However, since we cant control the order of destruction
		of singletons that originate in different translation units, somethings must be destroyed in a specific order.

		Therefore, this method must be called before application shutdown (or before the framework singletons go out of scope)
		to prevent exceptions.
	*/
	void OrderedStaticDestroy();

}}
