#pragma once

#include "MCCriticalSection.h"
#include "../Data/MCLinearBuffer.h"
#include <assert.h>
#include <map>

#include "../Data/MCLinearBuffer.h"

template <typename tManager>
class MCManagedLinearBufferHandle {
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

		if (this->_bufferAddress != MCLinearBuffer::InvalidAddress)
			tManager::Instance()->RemoveRef(*this);

		this->_bufferAddress = o._bufferAddress;
	}
	MCManagedLinearBufferHandle(MCManagedLinearBufferHandle&& o) {
		if (this == &o)
			return;

		if (this->_bufferAddress != MCLinearBuffer::InvalidAddress)
			tManager::Instance()->RemoveRef(*this);

		this->_bufferAddress = o._bufferAddress;

		o._bufferAddress = MCLinearBuffer::InvalidAddress;
	}
	MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle&) {
		if (this == &o)
			return *this;

		if (this->_bufferAddress != MCLinearBuffer::InvalidAddress)
			tManager::Instance()->RemoveRef(*this);

		this->_bufferAddress = o._bufferAddress;

		return *this;
	}
	MCManagedLinearBufferHandle& operator= (MCManagedLinearBufferHandle&&) {
		if (this == &o)
			return *this;

		if (this->_bufferAddress != MCLinearBuffer::InvalidAddress)
			tManager::Instance()->RemoveRef(*this);

		this->_bufferAddress = o._bufferAddress;

		o._bufferAddress = MCLinearBuffer::InvalidAddress;

		return *this;
	}
private:
	MCManagedLinearBufferHandle(MCLinearBufferAddress address) {
		_bufferAddress = address;
	}
private:
	MCLinearBufferAddress _bufferAddress;
	template <typename T>
	friend MCManagedLinearBufferHandleManager<tManaged, T>;
};

template <typename tManaged, unsigned int N>
class MCManagedLinearBufferHandleManager {
public:
	typedef MCManagedLinearBufferHandle<decltype(this)> HandleType;

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
		bool operator==(const ManagedContextItem& a, const ManagedContextItem& b) {
			return a.managedItem == b.managedItem;
		}
	};
	MCLinearBuffer<ManagedContextItem, N> _buffer;
	MCCriticalSectionLock       _lock;

private:
	friend HandleType;
};