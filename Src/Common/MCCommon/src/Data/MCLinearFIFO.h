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
			_pBuffer   = reinterpret_cast<unsigned char*>(malloc(sizeof(T)*N));
			_pNextPop  = _pBuffer;
			_pNextPush = _pBuffer;
			_pEnd      = _pBuffer + sizeof(T) * N;
		}
		~MCLinearFIFO() {
			if (_pBuffer)
				free(_pBuffer);
		}
		MCLinearFIFO(MCLinearFIFO&)              = delete;
		MCLinearFIFO(MCLinearFIFO&&)             = delete;
		MCLinearFIFO& operator= (MCLinearFIFO&)  = delete;
		MCLinearFIFO& operator= (MCLinearFIFO&&) = delete;
	public:
		void push(const T& t) {
			unsigned char* ptr = _pNextPush.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(ptr < _pEnd);
			MCASSERT(_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			*(reinterpret_cast<T*>(ptr)) = t;
		}

		T* push_to() {
			unsigned char* ptr = _pNextPush.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(ptr < _pEnd);
			MCASSERT(ptr >= _pBuffer);
			MCASSERT(_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			return reinterpret_cast<T*>(ptr);
		}

		bool pop(T* pT) {
			unsigned char* ptr = _pNextPop.fetch_add(sizeof(T));
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(ptr < _pNextPush.load());
			MCASSERT(!_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			*pT = *(reinterpret_cast<T*>(ptr));
			return (_pNextPop.load() < _pNextPush.load());
		}

		const T& peek() const {
#ifdef __DEBUG_MCLINEAR_FIFO__
			MCASSERT(!_writeMode);
#endif __DEBUG_MCLINEAR_FIFO__
			return *(reinterpret_cast<T*>(_pNextPop.load()));
		}

		void reset() {
			_pNextPush = _pNextPop = _pBuffer - sizeof(T);
			_writeMode = true;
		}

		void beginRead() {
			_writeMode = false;
		}

	private:
		unsigned char* _pBuffer;
		unsigned char* _pEnd;
		std::atomic<unsigned char*> _pNextPush;
		std::atomic<unsigned char*> _pNextPop;
#ifdef __DEBUG_MCLINEAR_FIFO__
		bool						_writeMode;
#endif __DEBUG_MCLINEAR_FIFO__
	};

}