#pragma once

namespace MC {

	template <typename pManagedType, typename ManagedContextItemType, typename ManagerType>
	class MCManagedHandle {
	public:
		MCManagedHandle() {
			_ptr = nullptr;
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

	private:
		inline const pManagedType Ptr() const { return _ptr; }
		inline void Initialize(pManagedType ptr) {
			assert(_ptr == nullptr);
			_ptr = ptr;
		}

	private:
		pManagedType _ptr;
		friend MCManagedHandleManager<pManagedType, ManagedContextItemType, ManagerType>;
	};

	template <typename pManagedType,typename ManagedContextItemType, typename DerivedType>
	class MCManagedHandleManager {
	public:
		MCManagedHandleManager() { MCCriticalSection::InitializeLock(&_lock); }
		virtual ~MCManagedHandleManager() {}

	protected:
		MCManagedHandle<pManagedType, ManagedContextItemType, DerivedType> CreateRef(pManagedType pKey, ManagedContextItemType &contextItem) {
			ManagedContextType managedContext;
			managedContext.RefCount    = 1;
			managedContext.ManagedItem = contextItem;
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_CreateRef, &_lock);
			_itemMap.emplace(pKey, managedContext);
			EXIT_CRITICAL_SECTION;
		}

		void AddRef(const MCManagedHandle<pManagedType, ManagedContextItemType, DerivedType>& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_AddRef, &_lock);
			ManagedContextType managedContext = _itemMap.find(handle.Ptr());
			managedContext.RefCount++
			EXIT_CRITICAL_SECTION; 
		}

		void RemoveRef(const MCManagedHandle<pManagedType, ManagedContextItemType, DerivedType>& handle) {
			ENTER_CRITICAL_SECTION(MCManagedHandleManager_RemoveRef, &_lock);
			ManagedContextType managedContext = _itemMap.find(handle.Ptr());			
			_itemMap.erase(handle.Ptr());
			managedContext.RefCount--;

			if(managedContext.RefCount == 0)
				ReferenceCountToZero(&managedContext);
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
		virtual void ReferenceCountToZero(ManagedContextType *pContext) = 0;
	};

}
