#pragma once

#include "../../Core/MCD3D12Core.h"

namespace MC {

	struct MCFrameRenderTargetInfo {
		ID3D12Resource *pRenderTarget;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPURenderTarget;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPURenderTarget;

		ID3D12Resource *pDepthStencil;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUDepthStencil;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUDepthStencil;
	};
}