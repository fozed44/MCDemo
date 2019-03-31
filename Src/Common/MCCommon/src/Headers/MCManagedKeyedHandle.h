#pragma once

#include "MCCriticalSection.h"
#include <assert.h>
#include <map>

namespace MC {

	template <typename T>
	class HasKeyMethod {
	private:
		typedef char YesType[1];
		typedef char NoType[2];

		template <typename C> static YesType& test(decltype(&C::Key));
		template <typename C> static NoType&  test(...);

	public:
		enum {
			value = sizeof(test<T>(0)) == sizeof(YesType)
		};
	};

	template <typename tKeyedHandle, typename tKey,  typename tDerivedManager>
	class MCManagedKeyedHandle {
		static_assert(HasKeyMethod<tKeyedHandle>::value, "tKeyedHandle must implement Key()");
	public:
		MCManagedKeyedHandle() {
			_handle = {};
		}		
		MCManagedKeyedHandle(MCManagedKeyedHandle&) = delete;
		MCManagedKeyedHandle& operator= (MCManagedKeyedHandle&) = delete;
		MCManagedKeyedHandle(MCManagedKeyedHandle&& o) {
			this->_handle = o._handle;
			o._handle = {};
		}
		MCManagedKeyedHandle& operator= (MCManagedKeyedHandle&& o) {
			if (this == &o)
				return *this;

			if (this->_handle.Key())
				tDerivedManager::Instance()->RemoveRef(*this);

			this->_handle = o._handle;
			o._handle = {};

			return *this;
		}
		~MCManagedKeyedHandle() {
			if (_handle.Key())
				tDerivedManager::Instance()->RemoveRef(*this);
		}

	private:
		MCManagedKeyedHandle(const tKeyedHandle& t) {
			_handle = t;
		}
		inline const tKeyedHandle& Handle() const { return _handle; }

		inline tKey Key() { return _handle.Key(); }

		/*inline void Initialize(tKeyedHandle handle) {
			assert(!_handle.Key());
			_handle = handle;
		}*/

	private:
		tKeyedHandle _handle;
		template<typename T, typename U, typename V, typename w>
		friend class MCManagedKeyedHandleManager;
		friend MCManagedKeyedHandle<tKeyedHandle, tKey, tDerivedManager>;
	};

	template <typename tKeyedHandle, typename tKey, typename tManagedItem, typename tDerived>
	class MCManagedKeyedHandleManager {
	public:
		typedef MCManagedKeyedHandle<tKeyedHandle, tKey, tDerived> HandleType;
	public:
		MCManagedKeyedHandleManager() { MCCriticalSection::InitializeLock(&_lock); }
		virtual ~MCManagedKeyedHandleManager() {}


	protected:
		HandleType CreateRef(tKeyedHandle& handle, tManagedItem &contextItem) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_CreateRef, &_lock);
			auto itemIterator = _itemMap.find(handle.Key());
			if (itemIterator == _itemMap.end())
				_itemMap.emplace(handle.Key(), ManagedContext{ contextItem, 1 });
			else
				itemIterator->second.RefCount++;
			return HandleType(handle);
			EXIT_CRITICAL_SECTION;
		}

		void RemoveRef(HandleType& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_RemoveRef, &_lock);
			auto itemIterator = _itemMap.find(handle.Key());
			assert(itemIterator->second.RefCount);
			itemIterator->second.RefCount--;
			if (itemIterator->second.RefCount == 0) {
				OnDestroyingManagedItem(&itemIterator->second.ManagedItem);
				_itemMap.erase(itemIterator);
			}
			EXIT_CRITICAL_SECTION;
		}

		const tKeyedHandle& UnwrapHandle(const HandleType& handle) const {
			return handle.Handle();
		}

	protected:
		struct ManagedContext {
			tManagedItem ManagedItem;
			int          RefCount;
		};
	private:
		std::map<tKey, ManagedContext> _itemMap;
		MCCriticalSectionLock _lock;

	protected:
		virtual void OnDestroyingManagedItem(tManagedItem *pManagedItem) {};
		template<typename t, typename u, typename v>
		friend class MCManagedKeyedHandle;

#ifdef MCMANAGEDHANDLE_UNIT_TEST_HELPERS
	public:
		int GetRefCount(const tKey key) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_GetRefCount, &_lock);
			return _itemMap[key].RefCount;
			EXIT_CRITICAL_SECTION;
		}
#endif MCMANAGEDHANDLE_UNIT_TEST_HELPERS
	};

}
