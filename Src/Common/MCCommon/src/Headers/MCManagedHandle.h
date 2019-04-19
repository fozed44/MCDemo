#pragma once

#include "MCCriticalSection.h"
#include <assert.h>
#include <map>

namespace MC {
	
	template <typename tManagedHandle, typename tDerivedManager>
	class MCManagedHandle {
	public:
		MCManagedHandle() {
			_handle = {};
		}		
		MCManagedHandle(MCManagedHandle&)             = delete;
		MCManagedHandle& operator= (MCManagedHandle&) = delete;		
		MCManagedHandle(MCManagedHandle&& o) {
			this->_handle = o._handle;
			o._handle = {};
		}
		MCManagedHandle& operator= (MCManagedHandle&& o) {
			if (this == &o)
				return *this;

			if (this->_handle)
				tDerivedManager::Instance()->RemoveRef(*this);

			this->_handle = o._handle;
			o._handle = {};

			return *this;
		}
		~MCManagedHandle() {
			if (_handle)
				tDerivedManager::Instance()->RemoveRef(*this);
		}

	private:
		MCManagedHandle(tManagedHandle t) {
			_handle = t;
		}
		inline const tManagedHandle& Handle() const { return _handle; }

	private:
		tManagedHandle _handle;
		template<typename T, typename U, typename V>
		friend class MCManagedHandleManager;
		friend MCManagedHandle<tManagedHandle, tDerivedManager>;
	};

	template <typename tManagedHandle,typename tManagedItem, typename tDerived>
	class MCManagedHandleManager {
	public:
		using HandleType = MCManagedHandle<tManagedHandle, tDerived>;
	public:
		MCManagedHandleManager() { MCCriticalSection::InitializeLock(&_lock); }
		virtual ~MCManagedHandleManager() {}

	protected:
		HandleType CreateRef(tManagedHandle handle, tManagedItem &contextItem) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_CreateRef, &_lock);
			auto itemIterator = _itemMap.find(handle);
			if (itemIterator == _itemMap.end())
				_itemMap.emplace(handle, ManagedContext{ contextItem, 1 });
			else
				itemIterator->second.RefCount++;
			return HandleType(handle);
			EXIT_CRITICAL_SECTION;
		}
		void RemoveRef(const MCManagedHandle<tManagedHandle, tDerived>& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_RemoveRef, &_lock);
			auto itemIterator = _itemMap.find(handle.Handle());
			assert(itemIterator->second.RefCount);
			itemIterator->second.RefCount--;
			if (itemIterator->second.RefCount == 0) {
				OnDestroyingManagedItem(&itemIterator->second.ManagedItem);
				_itemMap.erase(itemIterator);
			}
			EXIT_CRITICAL_SECTION;
		}
		const tManagedHandle& UnwrapHandle(const HandleType& handle) const noexcept {
			return handle.Handle();
		}

	protected:
		struct ManagedContext {
			tManagedItem ManagedItem;
			int          RefCount;
		};
	private:
		std::map<tManagedHandle, ManagedContext> _itemMap;
		MCCriticalSectionLock                    _lock;

	protected:
		virtual void OnDestroyingManagedItem(tManagedItem *pManagedItem) {};
		template<typename t, typename u>
		friend class MCManagedHandle;

#ifdef MCMANAGEDHANDLE_UNIT_TEST_HELPERS
	public:
		int GetRefCount(const tManagedHandle& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_GetRefCount, &_lock);
			return _itemMap[handle].RefCount;
			EXIT_CRITICAL_SECTION;
		}
#endif MCMANAGEDHANDLE_UNIT_TEST_HELPERS
	};

}
