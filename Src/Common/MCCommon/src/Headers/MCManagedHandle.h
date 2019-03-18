#pragma once

namespace MC {

	template <typename pManagedType, typename ManagerType>
	class MCManagedHandle {
	public:
		MCManagedHandle() {
			_pHandle = nullptr;
		}
		MCManagedHandle(MCManagedHandle&) = delete;
		MCManagedHandle& operator= (MCManagedHandel&) = delete;
		MCManagedHandle(MCManagedHandle&& o) {
			if (o._pHandle != nullptr)
				ManagerType::Instance()->RemoveRef(_pHandle);
		}
		MCManagedHandle& operator= (MCManagedHandle&& 0) {
			if (this->_pHandle != nullptr)
				ManagerType::Instance()->RemoveRef(_pHandle);

			if (o._pHandle != nullptr)
				ManagerType::Instance()->RemoveRef(_pHandle);
		}
		~MCManagedHandle() {
			if(_pHandle != nullptr)
				ManagerType::Instance()->RemoveRef(_pHandle);
		}

	private:
		inline const Handle() { return _pHandle; }

	private:
		pManagedType _pHandle;
		friend ManagerType;
	};

}
