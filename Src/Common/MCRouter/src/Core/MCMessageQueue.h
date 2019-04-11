#pragma once

#include "../../../MCCommon/src/Data/MCLinearFIFO.h"
#include "../../../MCLog/src/Headers/MCLog.h"
#include <functional>

namespace MC {

	constexpr int Pad32(int x) { return x / 4 * 4 + 4; }

	static_assert(sizeof(int) == 4, "sizeof(int) == 4 is required.");

	template<typename tMessage>
	class MCMessageQueueDataBuffer {
	public:
		template<typename T>
		using typed_ptr   = std::unique_ptr<T, void(MCMessageQueueDataBuffer::*)()>;
		using untyped_ptr = std::unique_ptr<char, void(MCMessageQueueDataBuffer::*)()>;
		MCMessageQueueDataBuffer(unsigned short size, const std::string& name) {
			_pBuffer = reinterpret_cast<char*>(malloc(size));
			_pEnd = _pBuffer + size;
			_pNextPush.store(_pBuffer);
			_writeOnly = true;
			_pushPtrCount.store(0);
			_popPtrCount.store(0);
			_name = name;
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

		template<typename U>
		typed_ptr<U> push_to() {
			char* ptr = _pNextPush.fetch_add(Pad32(sizeof(T)));
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + Pad32(sizeof(T)) < _pEnd));
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			_pushPtrCount++;
			return typed_ptr<T>(reinterpret_cast<T*>(ptr), decrementPushPtr);
		}

		untyped_ptr push_to(unsigned short size) {
			size = size / 4 * 4 + 4; // pad size to 32 bits. 
			char* ptr = _pNextPush.fetch_add(size);
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + size) < _pEnd);
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			_pushPtrCount++;
			return untyped_ptr(ptr, decrementPushPtr);
		}

		void  reset() {
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

			if(_popPtrCount.load())
				MC_WARN("MCMessageQueueDataBuffer::reset ({0}) {1:d} pop pointers are in use. Stalling thread.", _name, _popPtrCount);

			while (_popPtrCount.load())
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			_writeOnly = true;
		}

		void beginRead() {
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

			if(_pushPtrCount.load())
				MC_WARN("MCMessageQueueDataBuffer::beginRead ({0}) {1:d} push pointers are in use. Stalling thread.", _name, _pushPtrCount);

			while (_pushPtrCount.load()) 
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			_writeOnly = false;
		}

		char* buffer() {
			return _pBuffer;
		};

		unsigned short buffer_size() {
			return static_cast<unsigned short>(_pEnd - _pBuffer);
		}

		unsigned short free() {
			return static_cast<unsigned short>(_pEnd - _pNextPush);
		}

		template<typename T>
		typed_ptr TranslateMessageAddress(const tMessage& msg){
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT(!_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			_popPtrCount++;
			return typed_ptr<T>(reinterpret_cast<T*>(_pBuffer + msg.Address), decrementPopPtr);
		}

		untyped_ptr TranslateMessageAddress(const tMessage& msg) {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT(!_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			_popPtrCount++;
			return untyped_ptr(reinterpret_cast<char*>(_pBuffer + msg.Address), decrementPopPtr);
		}

		inline const std::string& Name() const { return _name; }

	private:
		std::string        _name;
		char*              _pBuffer;
		char*              _pEnd;
		std::atomic<char*> _pNextPush;
		bool               _writeOnly;

		void decrementPushPtr() { _pushPtrCount--; }

		void decrementPopPtr()  { _popPtrCount--; }

		std::atomic<unsigned int> _pushPtrCount;
		std::atomic<unsigned int> _popPtrCount;
	};

	/*******************************************************************************************/

	template<
		typename tMessage, /* The core type being queued. */
		unsigned int N1,   /* The size of the queues. */
		unsigned int N2	   /* The size of the memory buffers. */
	>
		class MCMessageQueue {
		using queue_type  = MCLinearFIFO<tMessage, N1>;
		using buffer_type = MCMessageQueueDataBuffer<tMessage>;
		using typed_ptr   = MCMessageQueueDataBuffer::typed_ptr;
		using untyped_ptr = MCMessageQueueDataBuffer::untyped_ptr;
		public:
			MCMessageQueue(const std::string& name) : _name{ name } {

				_frontQueue = std::make_unique<queue_type>();
				_backQueue  = std::make_unique<queue_type>();

				_frontBufffer = std::make_unique<queue_type>(_name + std::string(" buffer a"), N2);
				_backBuffer   = std::make_unique<queue_type>(_name + std::string(" buffer b"), N2);
			}
			~MCMessageQueue() {}
			MCMessageQueue(MCMessageQueue&) = delete;
			MCMessageQueue(MCMessageQueue&&) = delete;
			MCMessageQueue& operator= (MCMessageQueue&) = delete;
			MCMessageQueue& operator= (MCMessageQueue&&) = delete;
		public:

			void push(const tMessage& message) {
				_pFrontQueue->push(message);
			}

			untyped_ptr push_to(const tMessage& message, unsigned short dataSize) {
				untyped_ptr ptr = _pFrontBuffer->push_to(dataSize);
				tMessage* msg = _pFrontQueue->push_to();
				*msg = t;
				msg->Address = static_cast<MC_MESSAGE_ADDRESS>(ptr.get() - _pFrontBuffer->Buffer());
				return std::move(ptr);
			}

			template<typename T>
			typed_ptr<T> push_to(const tMessage& t) {
				typed_ptr<T> ptr = _pFrontBuffer->push_to<T>();
				tMessage* msg = _pFrontQueue->push_to();
				*msg = t;
				msg->Address = static_cast<MC_MESSAGE_ADDRESS>(ptr.get() - _pFrontBuffer->Buffer());
				return std::move(ptr);
			}



			bool pop(tMessage* pMessage) {
				return _backQueue->pop(pMessage);
			}

			const tMessage& peek() const {
				return _backQueue->pop();
			}

			void swap() {
				MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

				queue_type               *pTempQueue = _pFrontQueue;
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

			const std::string& Name() const { return _name; }

		private:

			std::string                  _name;

			std::unique_ptr<queue_type>  _pFrontQueue;
			std::unique_ptr<queue_type>  _pBackQueue;

			std::unique_ptr<buffer_type> _pFrontBuffer;
			std::unique_ptr<buffer_type> _pBackBuffer;
	};

}
