#pragma once

#include "MCCriticalSection.h"
#include "../Data/MCLinearBuffer.h"
#include <assert.h>
#include <map>

#include "../Data/MCLinearBuffer.h"

namespace MC {

	template <typename tManager, typename tLocal>
	class MCManagedLinearBufferHandle {
	public:
		using BufferType = typename tManager::BufferType;
	public:
		MCManagedLinearBufferHandle()
			: _bufferAddress{ MCLinearBuffer::InvalidAddress } {}
		~MCManagedLinearBufferHandle() {
			if (_bufferAddress != MCLinearBuffer::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);
		}
		MCManagedLinearBufferHandle(MCManagedLinearBufferHandle& o) {
			if (this == &o)
				return;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;
		}
		MCManagedLinearBufferHandle(MCManagedLinearBufferHandle&& o) {
			if (this == &o)
				return;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			o._bufferAddress = MCLinearBuffer::InvalidAddress;
		}
		MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle& o) {
			if (this == &o)
				return *this;

			if (this->_bufferAddress != BufferType::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData     = o._localData;

			return *this;
		}
		MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle&& o) {
			if (this == &o)
				return *this;

			if (this->_bufferAddress != MCLinearBuffer::InvalidAddress)
				tManager::Instance()->RemoveRef(*this);

			this->_bufferAddress = o._bufferAddress;
			this->_localData = o._localData;

			o._bufferAddress = MCLinearBuffer::InvalidAddress;

			return *this;
		}
	private:
		MCManagedLinearBufferHandle(MCLinearBufferAddress address) {
			_bufferAddress = address;
		}
	private:
		MCLinearBufferAddress _bufferAddress;
		tLocal                _localData;
		//template <typename T>
		//friend MCManagedLinearBufferHandleManager<tManaged, tLocal, T>;
		friend tManager;
	};

	template <typename tManaged, typename tLocalHandleData, unsigned int N>
	class MCManagedLinearBufferHandleManager {
	public:
		using HandleType  = MCManagedLinearBufferHandle<MCManagedLinearBufferHandleManager<tManaged, tLocalHandleData, N>, tLocalHandleData>;
		using ManagedType = tManaged;
	public:
		MCManagedLinearBufferHandleManager() {
			MCCriticalSection::InitializeLock(&_lock);
		}
		~MCManagedLinearBufferHandleManager() {}
		MCManagedLinearBufferHandleManager(MCManagedLinearBufferHandleManager&) = delete;
		MCManagedLinearBufferHandleManager(MCManagedLinearBufferHandleManager&&) = delete;
		MCManagedLinearBufferHandleManager& operator= (MCManagedLinearBufferHandleManager&) = delete;
		MCManagedLinearBufferHandleManager& operator= (MCManagedLinearBufferHandleManager&&) = delete;

	protected:
		HandleType AddRef(const tManaged& obj) {
			ENTER_CRITICAL_SECTION(MCManagedLinearBufferHandleManager_AddRef, &_lock);
			MCLinearBufferAddress currentPosition = _buffer.find(obj);
			if (currentPosition != MCLinearBuffer::InvalidAddress)
				return HandleType(MCLinearBufferAddress);
			return HandleType(_buffer.add(obj));
			EXIT_CRITICAL_SECTION;
		}
		void RemoveRef(const HandleType& handle) {
			ENTER_CRITICAL_SECTION(MCManagedLinearBufferHandleManager_RemoveRef, &_lock);
			_buffer.clear(handle._bufferAddress);
			EXIT_CRITICAL_SECTION;
		}
	private:
		struct ManagedContextItem {
			tManaged managedItem;
			int		 RefCount;
			bool operator==(const ManagedContextItem& o) {
				return this.managedItem == o.managedItem;
			}
		};
	protected:
		using BufferType = MCLinearBuffer<ManagedContextItem, N>;
	private:
		BufferType            _buffer;
		MCCriticalSectionLock _lock;

	private:
		friend HandleType;
	};

}