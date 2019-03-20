#pragma once

namespace MC {

	template <typename pManagedType, typename ManagerType>
	class MCManagedHandle {
	public:
		MCManagedHandle() {
			_ptr = nullptr;
		}
		MCManagedHandle(pManagedType t) {
			_ptr = t;
		}
		MCManagedHandle(MCManagedHandle&) = delete;
		MCManagedHandle& operator= (MCManagedHandle&) = delete;
		MCManagedHandle(MCManagedHandle&& o) {
			this->_ptr = o._ptr;
			o._ptr = nullptr;
		}
		MCManagedHandle& operator= (MCManagedHandle&& o) {
			if (this == &o)
				return *this;

			if (this->_ptr != nullptr)
				ManagerType::Instance()->RemoveRef(*this);

			this->_ptr = o._ptr;
			o._ptr = nullptr;

			return *this;
		}
		~MCManagedHandle() {
			if(_ptr != nullptr)
				ManagerType::Instance()->RemoveRef(*this);
		}

		inline const pManagedType Ptr() const { return _ptr; }

		inline void Initialize(pManagedType ptr) {
			assert(_ptr == nullptr);
			_ptr = ptr;
		}

	private:
		pManagedType _ptr;
	};

	template <typename pManagedType,typename ManagedContextItemType, typename DerivedType>
	class MCManagedHandleManager {
	public:
		MCManagedHandleManager() { MCCriticalSection::InitializeLock(&_lock); }
		virtual ~MCManagedHandleManager() {}

	protected:
		MCManagedHandle<pManagedType, DerivedType> CreateRef(pManagedType pKey, ManagedContextItemType &contextItem) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_CreateRef, &_lock);
			auto itemIterator = _itemMap.find(pKey);
			if (itemIterator == _itemMap.end())
				_itemMap.emplace(pKey, ManagedContextType{ contextItem, 1 });
			else
				itemIterator->second.RefCount++;
			EXIT_CRITICAL_SECTION;
			return MCManagedHandle<pManagedType, DerivedType>(pKey);
		}

		void RemoveRef(const MCManagedHandle<pManagedType, DerivedType>& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_RemoveRef, &_lock);
			auto itemIterator = _itemMap.find(handle.Ptr());
			assert(itemIterator->second.RefCount);
			itemIterator->second.RefCount--;
			if (itemIterator->second.RefCount == 0) {
				OnDestroyingManagedItem(&itemIterator->second.ManagedItem);
				_itemMap.erase(itemIterator);
			}
			EXIT_CRITICAL_SECTION;
		}


	protected:
		struct ManagedContextType {
			ManagedContextItemType ManagedItem;
			UINT                   RefCount;
		};
	private:
		std::map<pManagedType, ManagedContextType> _itemMap;
		MCCriticalSectionLock _lock;

	protected:
		virtual void OnDestroyingManagedItem(ManagedContextItemType *pManagedItem) {};
		template<typename t, typename u>
		friend class MCManagedHandle;
	};

}
