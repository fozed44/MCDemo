#pragma once

#include "../Core/MCD3D12Core.h"

using Microsoft::WRL::ComPtr;

namespace MC { namespace RootSignatures {

	ComPtr<ID3D12RootSignature> Default();

} }