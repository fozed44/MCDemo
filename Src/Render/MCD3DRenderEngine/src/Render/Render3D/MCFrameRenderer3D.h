#pragma once

#include "../../Core/MCD3D12Core.h"
#include "../../Core/MCD3D.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRenderer3D
	{
	public:
		MCFrameRenderer3D();
		~MCFrameRenderer3D();
		MCFrameRenderer3D(MCFrameRenderer3D&)              = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)             = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&&) = delete;

	public:
		void Render(const MCFrame3D& frame);

	private:
		ComPtr<ID3D12GraphicsCommandList> _pCommandList;
		ComPtr<ID3D12CommandAllocator>    _pCommandAllocator;

		ComPtr<ID3D12DescriptorHeap>      _CBVDescriptorHeap;

		std::unique_ptr<MCUploadBuffer<ObjectConstants>> _pObjectConstantBuffer;

		DirectX::XMFLOAT4X4 _worldMatrix;
		DirectX::XMFLOAT4X4 _viewMatrix;
		DirectX::XMFLOAT4X4 _projectionMatrix;

		D3D12_VIEWPORT _viewPort;
		D3D12_RECT     _scissorRect;
	};

}

