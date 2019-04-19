#pragma once

#include "../Core/MCD3D12Core.h"
#include <map>
#include "../../../../Common/MCCommon/Src/Headers/MCCriticalSection.h"
#include "../Core/MCREGlobals.h"

#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCShaderManager : public MCManagedHandleManager<ID3DBlob*, ComPtr<ID3DBlob>, MCShaderManager> {		
	public:
		using HShader = MCShaderManager::HandleType;
	public:
		MCShaderManager();
		MCShaderManager(MCShaderManager&)              = delete;
		MCShaderManager(MCShaderManager&&)             = delete;
		MCShaderManager& operator= (MCShaderManager&)  = delete;
		MCShaderManager& operator= (MCShaderManager&&) = delete;
		~MCShaderManager();
	private:
	public:
		inline static MCShaderManager* Instance() {
			return MCREGlobals::pShaderManager;
		}
	public:
		HShader Load(const char *pFilename);
		D3D12_SHADER_BYTECODE GetByteCode(const HShader& handle);
	};

	using HShader = MCShaderManager::HShader;
}
