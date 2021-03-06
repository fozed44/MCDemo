#pragma once

#include "../Headers/Utility.h"
#include "../Headers/GlobalSwitches.h"
#include "../Headers/MCCriticalSection.h"
#include "MCThreads.h"
#include <atomic>
#include <assert.h>
#include "stdlib.h"

namespace MC {

	template<typename T, unsigned int N>
	class MCLinearFIFO {
	public:
		MCLinearFIFO() {
			_pBuffer   = reinterpret_cast<char*>(malloc(sizeof(T)*N));
			_pNextPop  = _pBuffer;
			_pNextPush = _pBuffer;
			_pEnd      = _pBuffer + sizeof(T) * N;
		}
		~MCLinearFIFO() {
			if (_pBuffer)
				::free(_pBuffer);
		}
		MCLinearFIFO(MCLinearFIFO&)              = delete;
		MCLinearFIFO(MCLinearFIFO&&)             = delete;
		MCLinearFIFO& operator= (MCLinearFIFO&)  = delete;
		MCLinearFIFO& operator= (MCLinearFIFO&&) = delete;
	public:
		void push(const T& t) {
			char* ptr = _pNextPush.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(ptr < _pEnd);
			MCASSERT(_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			*(reinterpret_cast<T*>(ptr)) = t;
		}

		T* push_to() {
			char* ptr = _pNextPush.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(ptr < _pEnd);
			MCASSERT(ptr >= _pBuffer);
			MCASSERT(_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			return reinterpret_cast<T*>(ptr);
		}

		bool pop(T* pT) {
			if (_pNextPop.load() >= _pNextPush.load())
				return false;
			char* ptr = _pNextPop.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(!_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			*pT = *(reinterpret_cast<T*>(ptr));
			return true;
		}

		const T& peek() const {
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(!_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			return *(reinterpret_cast<T*>(_pNextPop.load()));
		}

		void reset() {
			_pNextPush = _pNextPop = _pBuffer;
#ifdef __DEBUG_MCLINEAR_FIFO__
			assert(_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
		}

#ifdef __DEBUG_MCLINEAR_FIFO__
		void allow_write(bool allowWrite) {
			_writeMode = allowWrite;
		}
#endif __DEBUG_MCLINEAR_FIFO__


		size_t buffer_size() const noexcept { return _pEnd - _pBuffer; }

		size_t free()        const noexcept { return _pEnd - _pBuffer; }

	private:
		char* _pBuffer;
		char* _pEnd;
		std::atomic<char*> _pNextPush;
		std::atomic<char*> _pNextPop;
#ifdef __DEBUG_MCLINEAR_FIFO__
		bool _writeMode;
#endif __DEBUG_MCLINEAR_FIFO__
	};

}