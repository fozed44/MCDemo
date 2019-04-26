#pragma once

#include "../Core/MCD3D12Core.h"
#include <map>
#include "../../../../Common/MCCommon/Src/Headers/MCCriticalSection.h"
#include "../Core/MCREGlobals.h"

#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	typedef enum class STANDARD_SHADER {
		DEFAULT_VERTEX = 0,
		DEFAULT_PIXEL  = 1
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

		/*	Load the shader file 'pFilename'. */
		HShader Load(const char *pFilename);

		/*	Retrieve a handle for the given standard shader. */
		HShader GetShaderHandle(STANDARD_SHADER standardShader);

		/* Retrieve a handle for the vertex type 'tVertex' */
		template<typename tVertex>
		HShader GetVertexShaderHandle();

		/*	Get the byte-code for a loaded shader. */
		D3D12_SHADER_BYTECODE GetByteCode(const HShader& handle);

		/* Get the byte-code for a standard shader. */
		D3D12_SHADER_BYTECODE GetByteCode(STANDARD_SHADER standardShader);

	private:
		std::map<STANDARD_SHADER, ComPtr<ID3DBlob>> _standardShaders;
		std::map<STANDARD_SHADER, std::string>      _filenameMap;

	private:
		/* Populates _filenameMap */
		void LoadFilenameMap();

		/* Loads a .cso file. */
		ComPtr<ID3DBlob> LoadFileInternal(const char *pFilename);
	};

	using HShader = MCShaderManager::HShader;
}
