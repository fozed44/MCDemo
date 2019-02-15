#pragma once

#include "MCD3D12Core.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
using Microsoft::WRL::ComPtr;

template<typename T>
class MCUploadBuffer {
public:
	MCUploadBuffer(ID3D12Device* pDevice, UINT elementCount, bool isConstantBuffer)
		: _isConstantBuffer(isConstantBuffer) {

		if (_isConstantBuffer)
			_elementByteSize = CalculateConstantBufferSize(sizeof(T));
		else
			_elementByteSize = sizeof(T);

		MCThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(_elementByteSize*elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(_buffer),
			&_buffer
		));

		MCThrowIfFailed(_buffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedData)));

		// The resource can stay mapped as long as the resource exists. However, the CPU must
		// take care not to write to the buffer while it is in use. Some sort of synchronization will
		// be required
	}
	MCUploadBuffer(const MCUploadBuffer& rhs) = delete;
	MCUploadBuffer& operator=(const MCUploadBuffer& rhs) = delete;
	~MCUploadBuffer() {
		if (_buffer != nullptr)
			_buffer->Unmap(0, nullptr);

		_mappedData = nullptr;
	}

	ID3D12Resource* Resource() const {
		return _buffer.Get();
	}

	/*
		Return the calculated size of the buffer elements. For non-constant buffers, this will just be 
		sizeof(T). For constant buffers this will be a multiple of the minimum hardware allocation size.
	*/
	UINT CalculatedSize() const {
		return _elementByteSize;
	}

	void CopyData(int elementIndex, const T& data) {
		memcpy(&_mappedData[elementIndex*_elementByteSize], &data, sizeof(T));
	}

private:
	UINT CalculateConstantBufferSize(UINT eleSize) {
		/*
			Constant buffers must be a multiple of the minimum hardware allocation size (usually 256 bytes),
			So round up to the nearest multiple of 256. We do this by adding 255 and then masking off the lower
			byte which stores all bits < 256

			Example: Suppose a byte size of 300:
			(300 + 255) & ~0x000000ff
			(555) & 0xFFFFFF00
			0x0000022B & 0xFFFFFF00 =
			0x00000200 = 512

		*/
		return (eleSize + 255) & ~255;
	}
private:
	ComPtr<ID3D12Resource> _buffer;
	BYTE*                  _mappedData;
	UINT                   _elementByteSize;
	bool                   _isConstantBuffer;
};