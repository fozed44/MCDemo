#include "MCShaderManager.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"
#include "../../../../Common/MCLog/src/Headers/MCLog.h"
#include "../Core/MCRenderObjects.h"

#include <fstream>

#include "../Core/d3dx12.h"
#include "d3dcompiler.h"
#include "../Core/MCREGlobals.h"

namespace MC {

#pragma region ctor

	MCShaderManager::MCShaderManager() {
		// Make sure the shader manager is only created a single time.
		assert(!MCREGlobals::pShaderManager);

		// Load the default filenames into the filename map.
		LoadFilenameMap();
	}

	MCShaderManager::~MCShaderManager() {}

#pragma endregion

#pragma region Default filename map.

	void MCShaderManager::LoadFilenameMap() {
		_filenameMap[STANDARD_SHADER::DEFAULT_PIXEL]  = "StandardPixel.cso";
		_filenameMap[STANDARD_SHADER::DEFAULT_VERTEX] = "StandardVertex.cso";
	}

#pragma endregion

#pragma region API

	ComPtr<ID3DBlob> MCShaderManager::LoadFileInternal(const char *pFilename) {
		RENDER_TRACE("Loading shader file '{0}'", pFilename);

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
		MCThrowIfFailed(D3DCreateBlob((size_t)size, &blob));

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();

		RENDER_TRACE("Loaded '{0}'", pFilename);

		return blob;
	}

	HShader MCShaderManager::Load(const char *pFilename) {		
		auto blob = LoadFileInternal(pFilename);
		return CreateRef(blob.Get(), blob);
	}

	D3D12_SHADER_BYTECODE MCShaderManager::GetByteCode(const HShader &handle) {
		auto pBlob = UnwrapHandle(handle);
		return D3D12_SHADER_BYTECODE{
			static_cast<BYTE*>(pBlob->GetBufferPointer()),
			pBlob->GetBufferSize()
		};
	}

	D3D12_SHADER_BYTECODE MCShaderManager::GetByteCode(STANDARD_SHADER standardShader) {
		auto handle = GetShaderHandle(standardShader);
		return GetByteCode(handle);
	}

	HShader MCShaderManager::GetShaderHandle(STANDARD_SHADER standardShader) {

		auto iterator = _standardShaders.find(standardShader);

		if (iterator != _standardShaders.end())
			return CreateRef((iterator->second).Get(), iterator->second);

		auto comPtr = LoadFileInternal(_filenameMap[standardShader].c_str());
		
		_standardShaders[standardShader] = comPtr;

		return CreateRef(comPtr.Get(), comPtr);
	}

	template<>
	HShader MCShaderManager::GetVertexShaderHandle<MCVertex1Color>() {
		return GetShaderHandle(STANDARD_SHADER::DEFAULT_VERTEX);
	}

#pragma endregion

}