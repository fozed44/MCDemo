#include "MCShaderManager.h"
#include "../../../../Common/MCCommon/src/Headers/Paths.h"
#include "../../../../Common/MCCommon/src/Headers/Utility.h"

#include <fstream>

#include "../Core/d3dx12.h"
#include "d3dcompiler.h"
#include "../Core/MCREGlobals.h"

namespace MC {

	MCShaderManager::MCShaderManager() {
		// Make sure the shader manager is only created a single time.
		assert(!MCREGlobals::pShaderManager);
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
		auto pBlob = handle.Handle();
		return D3D12_SHADER_BYTECODE{
			static_cast<BYTE*>(pBlob->GetBufferPointer()),
			pBlob->GetBufferSize()
		};
	}

}