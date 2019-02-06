#pragma once

//#include "dxgi1_6.h"
#include "d3d12.h"
#include "wrl.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class D3DWrapper {
	public:
		D3DWrapper();
		~D3DWrapper();

	public:
		void Init();

	private:

		ComPtr<ID3D12Device> _pDevice;
		ComPtr<ID3D12Fence>  _pFence;
		ComPtr<ID3D12CommandAllocator> _pCommandAllocator;
		ComPtr<ID3D12CommandQueue> _pCommandQueue;
		ComPtr<ID3D12CommandList> _pCommandList;

	private:
		void InitDevice();
		void InitFence();
		void InitCommandQueue();
		void InitCommandAllocator();
		void InitCommandList();
	};
}