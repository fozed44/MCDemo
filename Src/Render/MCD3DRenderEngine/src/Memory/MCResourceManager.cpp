#include "MCResourceManager.h"
#include "../Core/MCD3D.h"


namespace MC {

	void MCResourceManager::Initialize() {
		assert(MCD3D::Instance()->Initialized());
		assert(MCDXGI::Instance()->Initialized());

		// Create a new allocator
		MCThrowIfFailed(
			MCDXGI::Instance()->Get3DDevice()->
				CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					__uuidof(_pCommandAllocator),
					&_pCommandAllocator
				)
		);

		// Create a new command list.
		MCThrowIfFailed(
			MCDXGI::Instance()->Get3DDevice()->
			CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				_pCommandAllocator.Get(),
				nullptr,
				__uuidof(_pCommandList),
				&_pCommandList
			)
		);

	}

}
