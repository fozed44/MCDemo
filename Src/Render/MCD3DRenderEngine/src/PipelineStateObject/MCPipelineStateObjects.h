#pragma once

#include "../Core/MCD3D12Core.h"

using Microsoft::WRL::ComPtr;

namespace MC { namespace PSOs {

	ComPtr<ID3D12PipelineState> Default();

} }