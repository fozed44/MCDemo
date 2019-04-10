#pragma once

#include "MCLinearFIFO.h"


namespace MC {

	constexpr int Pad32(int x) { return x / 4 * 4 + 4; }

	static_assert(sizeof(int) == 4, "sizeof(int) == 4 is required.");

	class MCMessageQueueDataBuffer {
		MCMessageQueueDataBuffer(size_t size) {
			_pBuffer = reinterpret_cast<char*>(malloc(size));
			_pEnd    = _pBuffer + size;
			_pNextPush.store(_pBuffer);
			_writeOnly = true;
		}
		~MCMessageQueueDataBuffer() {
			if (_pBuffer)
				free(_pBuffer);
		}
		MCMessageQueueDataBuffer(MCMessageQueueDataBuffer&) = delete;
		MCMessageQueueDataBuffer(MCMessageQueueDataBuffer&&) = delete;
		MCMessageQueueDataBuffer& operator= (MCMessageQueueDataBuffer&) = delete;
		MCMessageQueueDataBuffer& operator= (MCMessageQueueDataBuffer&&) = delete;
	public:

		template<typename T>
		T* push_to() {
			char* ptr = _pNextPush.fetch_add(Pad32(sizeof(T)));
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + Pad32(sizeof(T)) < _pEnd));
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			return reinterpret_cast<T*>(ptr);
		}

		char* push_to(unsigned short size) {
			size = size / 4 * 4 + 4; // pad size to 32 bits.
			char* ptr = _pNextPush.fetch_add(size);
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + size) < _pEnd);
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			return ptr;
		}

		void  reset()     { MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN); _writeOnly = true; }
		char* beginRead() { MCTHREAD_ASSERT(MC_THREAD_ASSERT_MAIN); _writeOnly = false; return _pBuffer; }

		char* Buffer() {
			return _pBuffer;
		};

	private:
		char*              _pBuffer;
		char*              _pEnd;
		std::atomic<char*> _pNextPush;
		bool               _writeOnly;
	};

	template<typename T, unsigned int N1, unsigned int N2, unsigned short (*F1)(const T*), void (*F2)(T*, unsigned short P)>
	class MCMessageQueue {
		using queue_type = MCLinearFIFO<T, N>;
		using translateMessagePtrFrom = F1;
		using translateMessagePtrTo   = F2;
	public:
		MCMessageQueue() {
			_frontQueue = FRONT_QUEUE;
			_backQueue  = BACK_QUEUE;
		}
		~MCMessageQueue() {}
		MCMessageQueue(MCMessageQueue&)              = delete;
		MCMessageQueue(MCMessageQueue&&)             = delete;
		MCMessageQueue& operator= (MCMessageQueue&)  = delete;
		MCMessageQueue& operator= (MCMessageQueue&&) = delete;
	public:
		void push(const T& t) {
			_pFrontQueue->push(t);
		}

		char* push(const T& t, unsigned short dataSize) {
			char* ptr = _pFrontBuffer->push_to(dataSize);
			unsigned char delta = static_cast<unsigned char>(ptr - _pFrontBuffer->Buffer());
			auto msg = _pFrontQueue->push_to();
			*msg = t;
			translateMessagePtrTo(msg, delta);
			return 
		}

		T& push_to() {
			return _frontQueue->push_to(t);
		}

		bool pop(T* pT) {
			return _backQueue->back
		}

		const T& peek() const {
		}

		void reset() {
		}

		void beginRead() {
		}

		void swap() {
			queue_type               *pTempQueue  = _pFrontQueue;
			MCMessageQueueDataBuffer *pTempBuffer = _pFrontBuffer;

			_pFrontQueue = _pBackQueue;
			_pBackQueue  = pTempQueue;

			_pFrontBuffer = _pBackBuffer;
			_pBackBuffer  = _pTempBuffer;

			_pFrontQueue->reset();
			_pFrontBuffer->reset();

			_pBackQueue->beginRead();
			_pBackBuffer->beginRead();
		}

	private:
		queue_type *_pFrontQueue;
		queue_type *_pBackQueue;

		MCMessageQueueDataBuffer *_pFrontBuffer;
		MCMessageQueueDataBuffer *_pBackBuffer;

		queue_type FRONT_QUEUE;
		queue_type BACK_QUEUE;

		MCMessageQueueDataBuffer FRONT_BUFFER;
		MCMessageQueueDataBuffer BACK_BUFFER;
	}

}
