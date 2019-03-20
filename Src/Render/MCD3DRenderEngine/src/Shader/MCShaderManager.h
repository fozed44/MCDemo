#pragma once

#include "../Core/MCD3D12Core.h"
#include <map>
#include "../../../../Common/MCCommon/Src/Headers/MCCriticalSection.h"

#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {	

	class MCShaderManager;

	typedef MCManagedHandle<ID3DBlob*, MCShaderManager> MCShaderHandle;

	class MCShaderManager : public MCManagedHandleManager<ID3DBlob*, ComPtr<ID3DBlob>, MCShaderManager> {
	public:
		MCShaderManager(MCShaderManager&)              = delete;
		MCShaderManager(MCShaderManager&&)             = delete;
		MCShaderManager& operator= (MCShaderManager&)  = delete;
		MCShaderManager& operator= (MCShaderManager&&) = delete;
		~MCShaderManager();
	private:
		MCShaderManager();
	public:
		inline static MCShaderManager* Instance() {
			static MCShaderManager instance;
			return &instance;
		}
	public:
		MCShaderHandle Load(const char *pFilename);
		D3D12_SHADER_BYTECODE GetByteCode(const MCShaderHandle& handle);
	};

}
