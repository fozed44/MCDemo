#pragma once

#include "MCCriticalSection.h"
#include "../Data/MCLinearBuffer.h"
#include "../../../MCCommon/src/Headers/Utility.h"
#include <memory>
#include <assert.h>
#include <map>

#include "../Analyzers/MCLinearBufferAnalyzer.h"

namespace MC {
	template <typename tManaged, typename tLocalHandleData, typename tDerived, unsigned int N>
	class MCManagedLinearBufferHandleManager;

	template <typename tManager, typename tLocal>
	class MCManagedLinearBufferHandle {
	public:
		using BufferType = typename tManager::BufferType;
	public:
		MCManagedLinearBufferHandle()
			: _bufferAddress{ BufferType::InvalidAddress } {}
		~MCManagedLinearBufferHandle() {
			if (_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);
		}
		MCManagedLinearBufferHandle(MCManagedLinearBufferHandle& o) {
			if (this == &o)
				return;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			tManager::Instance()->AddRef(*this);
		}
		MCManagedLinearBufferHandle(MCManagedLinearBufferHandle&& o) {
			if (this == &o)
				return;

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			o._bufferAddress = BufferType::InvalidAddress;
		}
		MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle& o) {
			if (this == &o)
				return *this;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			tManager::Instance()->AddRef(*this);

			return *this;
		}
		MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle&& o) {
			if (this == &o)
				return *this;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			o._bufferAddress = BufferType::InvalidAddress;

			return *this;
		}
		MCLinearBufferAddress get_buffer_address() const { return _bufferAddress; }
	private:
		MCManagedLinearBufferHandle(MCLinearBufferAddress address, const tLocal& localData) {
			_bufferAddress = address;
			_localData     = localData;
		}
	private:
		MCLinearBufferAddress _bufferAddress;
		tLocal                _localData;
		friend tManager;
		template <typename T, typename U, typename V, unsigned int N>
		friend class MCManagedLinearBufferHandleManager;
	};

	template <typename tManaged, typename tLocalHandleData, typename tDerived, unsigned int N>
	class MCManagedLinearBufferHandleManager {
	public:
		using HandleType  = MCManagedLinearBufferHandle<tDerived, tLocalHandleData>;
		using ManagedType = tManaged;
	public:
		MCManagedLinearBufferHandleManager() {
			MCCriticalSection::InitializeLock(&_lock);
		}
		virtual ~MCManagedLinearBufferHandleManager() {}
		MCManagedLinearBufferHandleManager(MCManagedLinearBufferHandleManager&)              = delete;
		MCManagedLinearBufferHandleManager(MCManagedLinearBufferHandleManager&&)             = delete;
		MCManagedLinearBufferHandleManager& operator= (MCManagedLinearBufferHandleManager&)  = delete;
		MCManagedLinearBufferHandleManager& operator= (MCManagedLinearBufferHandleManager&&) = delete;

	protected:
		/* Call CreateNewItem to add a copy of obj to the linear buffer. A new handle is created using
		   localHandleData as the data local to the handle. The new handle is returned to the caller. */
		HandleType CreateNewItem(const ManagedType& obj, const tLocalHandleData localHandleData) {
			MCLinearBufferAddress address;

			ENTER_CRITICAL_SECTION(MCManagedLinearBufferHandle_CreateNewItem, &_lock);

			address = _buffer.add(ManagedContextItem {
				obj,
				1
			});

			EXIT_CRITICAL_SECTION;

			return std::move(HandleType(address, localHandleData));
		}

		/* Call AddRef to add a reference to buffer item referenced by 'handle'.
		   NOTE:
				This method should only be called by the handle code, not the derived
				class. It is strictly used to handle multiple copies of handles to the
				buffer item. */
		void AddRef(const HandleType& handle) {
			ENTER_CRITICAL_SECTION(MCManagedLinearBufferHandleManager_AddRef, &_lock);
			ManagedContextItem *ptr = _buffer.get(handle.get_buffer_address());
			ptr->RefCount++;
			EXIT_CRITICAL_SECTION;
		}

		/* Call RemoveRef to remove a reference to the buffer item referenced by 'handle'.
		   NOTE:
			This method should only be called by the handle code. not the derived class.
			It is strictly used to manage the destruction of managed handles. */
		void RemoveRef(const HandleType& handle) {
			ENTER_CRITICAL_SECTION(MCManagedLinearBufferHandleManager_RemoveRef, &_lock);
			ManagedContextItem *ptr = _buffer.get(handle.get_buffer_address());
			auto newCount = --ptr->RefCount;

			assert(newCount >= 0);

			if(newCount == 0)
				_buffer.clear(handle.get_buffer_address());
			EXIT_CRITICAL_SECTION;
		}

		/* Get the address of the item managed by the handle. */
		ManagedType *GetManagedItem(const HandleType& handle) {
			return _buffer.get(handle.get_buffer_address());
		}
	private:
		struct ManagedContextItem {
			ManagedType managedItem;
			int		    RefCount;
			bool operator==(const ManagedContextItem& o) {
				return this.managedItem == o.managedItem;
			}
		};
	protected:
		using BufferType = MCLinearBuffer<ManagedContextItem, N>;
	public: /* analyzers */
#ifdef _DEBUG
		std::unique_ptr<MCIAnalyzer> GetBufferAnalyzer() { return std::make_unique<MCLinearBufferAnalyzer<ManagedContextItem, N>>(&_buffer); }
#endif _DEBUG
	private:
		BufferType            _buffer;
		MCCriticalSectionLock _lock;

	private:
		friend HandleType;
	};

}