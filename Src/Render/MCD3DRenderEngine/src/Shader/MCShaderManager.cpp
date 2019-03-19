#include "MCShaderManager.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

#include <fstream>

#include "../Core/d3dx12.h"
#include "d3dcompiler.h"


using Microsoft::WRL::ComPtr;

namespace MC {

#pragma region MCShaderHandle

	//MCShaderHandle::MCShaderHandle() {
	//	_handle = MCShaderHandle::INVALID_HANDLE;
	//}

	//MCShaderHandle::MCShaderHandle(MCShaderHandle&& handle) {
	//	this->_handle = handle._handle;

	//	// Invalidate the source handle so that when the source object is destructed, the destructor does
	//	// not call RemoveRef for this handle.
	//	handle._handle = MCShaderHandle::INVALID_HANDLE;
	//}

	//MCShaderHandle& MCShaderHandle::operator=(MCShaderHandle&& handle) {
	//	this->_handle = handle._handle;

	//	handle._handle = MCShaderHandle::INVALID_HANDLE;
	//	return *this;
	//}

	//MCShaderHandle::~MCShaderHandle() {
	//	// Really, the only time _handle should == INVALID_HANDLE is if this object has been moved to another
	//	// object, or has just been created.
	//	if (_handle != MCShaderHandle::INVALID_HANDLE)
	//		MCShaderManager::Instance()->RemoveRef(*this);
	//}

	//UINT MCShaderHandle::GetNextHandle() {
	//	static std::atomic<UINT> _nextHandle;

	//	// Store the result of an atomic increment.
	//	UINT result = _nextHandle++;

	//	// Make sure the new handle is not equal to MCSaderHandle::INVALID_HANDLE, if it is
	//	// atomic increment once more.
	//	if (result == MCShaderHandle::INVALID_HANDLE)
	//		result = _nextHandle++;

	//	return result;
	//}

	//void MCShaderHandle::Initialize() {
	//	assert(_handle == MCShaderHandle::INVALID_HANDLE);
	//	_handle = GetNextHandle();
	//}

#pragma endregion

#pragma region MCShaderManager

	//MCShaderManager::MCShaderManager() {
	//}

	//MCShaderManager::~MCShaderManager() {}
	//MCShaderHandle MCShaderManager::Load(const char *pFilename) {

	//	// TODO:
	//	//	This method has zero error checking.

	//	auto fullFilename = std::string(Paths::ShaderDirectory()) + pFilename;

	//	std::ifstream fin(fullFilename.c_str(), std::ios::binary);

	//	fin.seekg(0, std::ios_base::end);
	//	std::ifstream::pos_type size = (int)fin.tellg();
	//	fin.seekg(0, std::ios_base::beg);

	//	if (!fin.good())
	//		MCTHROW(std::string("Error opening file ") + pFilename);

	//	ComPtr<ID3DBlob> blob;
	//	MCThrowIfFailed(D3DCreateBlob(size, &blob));

	//	fin.read((char*)blob->GetBufferPointer(), size);
	//	fin.close();
	//	
	//	return CreateRef(blob);
	//}

	//D3D12_SHADER_BYTECODE MCShaderManager::GetByteCode(const MCShaderHandle &handle) {
	//	D3D12_SHADER_BYTECODE result;
	//	auto pBlob = handle.Ptr();
	//	result.pShaderBytecode = static_cast<BYTE*>(pBlob->GetBufferPointer());
	//	result.BytecodeLength  = pBlob->GetBufferSize();
	//	return result;
	//}

	//MCShaderHandle MCShaderManager::CreateRef(ComPtr<ID3DBlob> shader) {
	//	MCShaderHandle newHandle;
	//	newHandle.Initialize(shader.Get());
	//	MCShaderContext newContext{ shader, 1 };
	//	ENTER_CRITICAL_SECTION(CreateShaderRef, &_lock);
	//	_shaders.emplace(shader.Get(), newContext);
	//	EXIT_CRITICAL_SECTION;
	//	return newHandle;
	//}

	//void MCShaderManager::AddRef(const MCShaderHandle& handle) {
	//	ENTER_CRITICAL_SECTION(AddShaderRef, &_lock);
	//	_shaders[handle.Ptr()]._refCount++;
	//	EXIT_CRITICAL_SECTION;
	//}

	//void MCShaderManager::RemoveRef(const MCShaderHandle& handle) {
	//	ENTER_CRITICAL_SECTION(AddShaderRef, &_lock);
	//	auto context = _shaders[handle.Ptr()];
	//	context._refCount--;
	//	if (!context._refCount) {
	//		_shaders.erase(handle.Ptr());
	//	}
	//	EXIT_CRITICAL_SECTION;
	//}

#pragma endregion

#pragma region Test Code

	MCShaderManager::MCShaderManager() {
	}

	MCShaderManager::~MCShaderManager() {}

	MCShaderHandle MCShaderManager::Load(const char *pFilename) {

		// TODO:
		//	This method has zero error checking.

		auto fullFilename = std::string(Paths::ShaderDirectory()) + pFilename;

		std::ifstream fin(fullFilename.c_str(), std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		if (!fin.good())
			MCTHROW(std::string("Error opening file ") + pFilename);

		ComPtr<ID3DBlob> blob;
		MCThrowIfFailed(D3DCreateBlob(size, &blob));

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();
		
		return CreateRef(blob.Get(), blob);
	}

	D3D12_SHADER_BYTECODE MCShaderManager::GetByteCode(const MCShaderHandle &handle) {
		D3D12_SHADER_BYTECODE result;
		auto pBlob = handle.Ptr();
		result.pShaderBytecode = static_cast<BYTE*>(pBlob->GetBufferPointer());
		result.BytecodeLength  = pBlob->GetBufferSize();
		return result;
	}

	void MCShaderManager::ReferenceCountToZero(ManagedContextType *pContext) {
		// nothing;
	}

	/*MCShaderHandle MCShaderManager::CreateRef(ComPtr<ID3DBlob> shader) {
		MCShaderHandle newHandle;
		newHandle.Initialize(shader.Get());
		MCShaderContext newContext{ shader, 1 };
		ENTER_CRITICAL_SECTION(CreateShaderRef, &_lock);
		_shaders.emplace(shader.Get(), newContext);
		EXIT_CRITICAL_SECTION;
		return newHandle;
	}

	void MCShaderManager::AddRef(const MCShaderHandle& handle) {
		ENTER_CRITICAL_SECTION(AddShaderRef, &_lock);
		_shaders[handle.Ptr()]._refCount++;
		EXIT_CRITICAL_SECTION;
	}

	void MCShaderManager::RemoveRef(const MCShaderHandle& handle) {
		ENTER_CRITICAL_SECTION(AddShaderRef, &_lock);
		auto context = _shaders[handle.Ptr()];
		context._refCount--;
		if (!context._refCount) {
			_shaders.erase(handle.Ptr());
		}
		EXIT_CRITICAL_SECTION;
	}*/

#pragma endregion

}