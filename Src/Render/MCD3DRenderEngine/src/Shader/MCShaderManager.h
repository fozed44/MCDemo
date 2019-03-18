#pragma once

#include "../Core/MCD3D12Core.h"
#include <map>
#include "../../../../Common/MCCommon/Src/Headers/MCCriticalSection.h"

#include "../../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

using Microsoft::WRL::ComPtr;

namespace MC {
	
	class MCShaderManager;

	class MCShaderHandle {
	public:
		/*
			Shader handles are always constructed with the value of INVALID_HANDLE. The shader must be initialized by calling
			Initialize. Notice that the Initialize method is a private method, which can only be accessed by MCShaderManager 
			(MCShaderManager is friend class).

			This setup allows other classes to own MCShaderHandles (have value copies) that are invalid when the owning class
			is constructed. The owning class must get a valid handle from the shader manager before using the handle.
		*/
		MCShaderHandle();
		MCShaderHandle(MCShaderHandle&)             = delete;
		MCShaderHandle& operator= (MCShaderHandle&) = delete;
		MCShaderHandle& operator= (MCShaderHandle&& handle);
		MCShaderHandle(MCShaderHandle&& handle);
		~MCShaderHandle();
	private:
		/*
			INVALID_HANDLE is used internally to represent a dead handle. It is used to identify a handle
			as dead during transient states, for example after a move, before the constructor is called.

			Trying to create a handle with the value of INVALID_HANDLE will fire an assertion.
		*/
		const static UINT INVALID_HANDLE = 0;

		/*
			Gets the next valid handle. This the static method that is used by the Initialize method.
		*/
		static UINT GetNextHandle();

		/*
			Initializes this object to the next valid handle. This method can only be called on a newly create object. This
			in ensured by making sure that the current handle of this object == INVALID_HANDLE
		*/
		void Initialize();

	private:
		UINT _handle;
	private:
		friend MCShaderManager;
	};

	typedef MCManagedHandle<ComPtr<ID3DBlob>*, MCShaderManager> MCShaderManagedHandle;

	class MCShaderManager {
	public:
		MCShaderManager(MCShaderManager&)             = delete;
		MCShaderManager(MCShaderManager&&)            = delete;
		MCShaderManager& operator= (MCShaderManager&) = delete;
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

	private:
		MCShaderHandle CreateRef(ComPtr<ID3DBlob> shader);
		void AddRef(const MCShaderHandle& handle);
		void RemoveRef(const MCShaderHandle& handle);

	private:
		struct MCShaderContext {
			ComPtr<ID3DBlob> _pShader;
			UINT _refCount;
		};
		std::map<UINT, MCShaderContext> _shaders;
		std::map<ComPtr<ID3DBlob>*, MCShaderContext> _
		MCCriticalSectionLock _lock;
	private:
		friend MCShaderHandle;		
	};

}
