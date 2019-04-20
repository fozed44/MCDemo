#pragma once

#include "../Core/MCD3D12Core.h"
#include <map>
#include "../../../../Common/MCCommon/Src/Headers/MCCriticalSection.h"
#include "../Core/MCREGlobals.h"

#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	typedef enum class STANDARD_SHADER {
		DEFAULT = 0
	} STANDARD_SHADER;

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
	public: /* Instance required by MCManagedHandleManager */
		inline static MCShaderManager* Instance() { return MCREGlobals::pShaderManager; }
	public: /* API */
		HShader Load(const char *pFilename);
		HShader GetShaderHandle(STANDARD_SHADER standardShader);
		D3D12_SHADER_BYTECODE GetByteCode(const HShader& handle);
		
	private:
		std::map<STANDARD_SHADER, ComPtr<ID3DBlob*>> _standardShaders;
	};

	using HShader = MCShaderManager::HShader;
}
