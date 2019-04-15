#pragma once

#include "../../../MCCommon/src/Data/MCLinearFIFO.h"
#include "../../../MCLog/src/Headers/MCLog.h"
#include "../../../MCCommon/src/Data/MCResult.h"
#include <algorithm>

namespace MC {

	constexpr int Pad32(int x) { return (x-1) / 4 * 4 + 4; }

	static_assert(sizeof(int) == 4, "sizeof(int) == 4 is required.");

	class MCMessageQueueDataBuffer {
	public:
		MCMessageQueueDataBuffer(const std::string& name, unsigned short size) {
			_pBuffer = reinterpret_cast<char*>(malloc(size));
			_pEnd    = _pBuffer + size;
			_name    = name;

			_pNextPush.store(_pBuffer);

#ifdef __DEBUG_ROUTING__
			_writeOnly = true;
#endif __DEBUG_ROUTING__

		}
		~MCMessageQueueDataBuffer() {
			if (_pBuffer)
				::free(_pBuffer);
		}
		MCMessageQueueDataBuffer(MCMessageQueueDataBuffer&)              = delete;
		MCMessageQueueDataBuffer(MCMessageQueueDataBuffer&&)             = delete;
		MCMessageQueueDataBuffer& operator= (MCMessageQueueDataBuffer&)  = delete;
		MCMessageQueueDataBuffer& operator= (MCMessageQueueDataBuffer&&) = delete;
	public:

		template<typename T>
		T* push_to() {
			char* ptr = _pNextPush.fetch_add(Pad32(sizeof(T)));
#ifdef __DEBUG_ROUTING__
			MCASSERT((ptr + Pad32(sizeof(T)) < _pEnd));
			MCASSERT(_writeOnly);
#endif __DEBUG_ROUTING__
			return reinterpret_cast<T*>(ptr);
		}

		char* push_to(unsigned short size) {
			char* ptr = _pNextPush.fetch_add(Pad32(size));
#ifdef __DEBUG_ROUTING__
			MCASSERT((ptr + Pad32(size)) < _pEnd);
			MCASSERT(_writeOnly);
#endif __DEBUG_ROUTING__
			return ptr;
		}

		char* get_buffer() {
			return _pBuffer;
		}

		unsigned short buffer_size() {
			return static_cast<unsigned short>(_pEnd - _pBuffer);
		}

		unsigned short free() {
			return static_cast<unsigned short>(_pEnd - _pNextPush);
		}

		inline const std::string& Name() const { return _name; }

#ifdef __DEBUG_ROUTING__
		void allow_write(bool allowWrite) { 
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN); 
			_writeOnly = allowWrite;
		}
#endif __DEBUG_ROUTING__

		void reset() {
#ifdef __DEBUG_ROUTING__
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
			assert(_writeOnly);
#endif __DEBUG_ROUTING__
			_pNextPush.store(_pBuffer);
		}

		char* allocate(unsigned short dataSize) {
			char* ptr = _pNextPush.fetch_add(Pad32(dataSize));
#ifdef __DEBUG_ROUTING__
			MCASSERT(_pNextPush.load() <= _pEnd);
			MCASSERT(_writeOnly);
#endif __DEBUG_ROUTING__
			return ptr;
		}

	private:
		std::string        _name;
		char*              _pBuffer;
		char*              _pEnd;
		std::atomic<char*> _pNextPush;
#ifdef __DEBUG_ROUTING__
		bool               _writeOnly;
#endif __DEBUG_ROUTING__
	};	

	/*******************************************************************************************/

	template<
		typename tMessage, /* The core type being queued. */
		unsigned int N1    /* The size of the queues. */
	>
		class MCMessageQueue {
		using queue_type = MCLinearFIFO<tMessage, N1>;
		public:
			MCMessageQueue(const std::string& name) : _name{ name } {
				_pFrontQueue = std::make_unique<queue_type>();
				_pBackQueue  = std::make_unique<queue_type>();
			}
			~MCMessageQueue() {}
			MCMessageQueue(MCMessageQueue&) = delete;
			MCMessageQueue(MCMessageQueue&&) = delete;
			MCMessageQueue& operator= (MCMessageQueue&) = delete;
			MCMessageQueue& operator= (MCMessageQueue&&) = delete;
		public: /* push, push_to */
			
			void push(const tMessage& message) {
				_pFrontQueue->push(message);
			}

		public: /* pop, peek */
			bool pop(tMessage* pMessage) {
				return _pBackQueue->pop(pMessage);
			}

			const tMessage& peek() const {
				return _pBackQueue->peek();
			}

#pragma region swap

			void swap() {

#ifdef __DEBUG_ROUTING__
				_pFrontQueue->allow_write(false);
				_pBackQueue ->allow_write(true);
#endif __DEBUG_ROUTING__

				_pFrontQueue.swap(_pBackQueue);
				_pFrontQueue->reset();
			}

#pragma endregion

#pragma region memory reporting

			size_t front_queue_size() const { return _pFrontQueue->buffer_size(); }
			size_t front_queue_free() const { return _pFrontQueue->free(); }

			size_t back_queue_size() const { return _pBackQueue->buffer_size(); }
			size_t back_queue_free() const { return _pBackQueue->free(); }

#pragma endregion

			const std::string& Name() const { return _name; }

		private:

			std::string                  _name;

			std::unique_ptr<queue_type>  _pFrontQueue;
			std::unique_ptr<queue_type>  _pBackQueue;

	};

	/*******************************************************************************************/

	template<
		typename tMessage, /* The core type being queued. */
		unsigned int N1,   /* The size of the queues. */
		unsigned int N2	   /* The size of the memory buffers. */
	>
		class MCMessageQueueWithMemoryBuffer {
		using queue_type = MCLinearFIFO<tMessage, N1>;
		public:
			MCMessageQueueWithMemoryBuffer(const std::string& name) : _name{ name } {
				_pFrontQueue = std::make_unique<queue_type>();
				_pBackQueue  = std::make_unique<queue_type>();

				_pFrontBuffer = std::make_unique<MCMessageQueueDataBuffer>(_name + std::string(" buffer a"), N2);
				_pBackBuffer  = std::make_unique<MCMessageQueueDataBuffer>(_name + std::string(" buffer b"), N2);

				_pCurrentReadBase  = _pFrontBuffer->get_buffer();
				_pCurrentWriteBase = _pBackBuffer->get_buffer();
			}
			~MCMessageQueueWithMemoryBuffer() {}
			MCMessageQueueWithMemoryBuffer(MCMessageQueueWithMemoryBuffer&)              = delete;
			MCMessageQueueWithMemoryBuffer(MCMessageQueueWithMemoryBuffer&&)             = delete;
			MCMessageQueueWithMemoryBuffer& operator= (MCMessageQueueWithMemoryBuffer&)  = delete;
			MCMessageQueueWithMemoryBuffer& operator= (MCMessageQueueWithMemoryBuffer&&) = delete;
		public: /* push, push_to */

				/*	Push a normal message. (A message that does not use an external Address) */
			void push(const tMessage& message) {
				_pFrontQueue->push(message);
			}

			char* push_to(const tMessage& message, unsigned short dataSize) {
				char*     ptr = _pFrontBuffer->push_to(dataSize);
				tMessage* msg = _pFrontQueue->push_to();
				*msg = message;
				msg->pAddress = ptr;
				return ptr;
			}

			template<typename T>
			T* push_to(const tMessage& t) {
				char*     ptr = _pFrontBuffer->push_to(sizeof(T));
				tMessage* msg = _pFrontQueue->push_to();
				*msg = t;
				msg->pAddress = ptr;
				return reinterpret_cast<T*>(ptr);
			}

		public: /* Arbitrary storage */

			char* allocate(unsigned short size) {
				return _pFrontBuffer->allocate(size);
			}

		public: /* pop, peek */
			bool pop(tMessage* pMessage) {
				return _pBackQueue->pop(pMessage);
			}

			const tMessage& peek() const {
				return _pBackQueue->peek();
			}

#pragma region swap

			void swap() {

#ifdef __DEBUG_ROUTING__
				_pFrontBuffer->allow_write (false);
				_pFrontQueue ->allow_write(false);

				_pBackBuffer->allow_write (true);
				_pBackQueue ->allow_write(true);
#endif __DEBUG_ROUTING__

				_pFrontBuffer.swap(_pBackBuffer);
				_pFrontQueue .swap(_pBackQueue);

				_pCurrentReadBase  = _pFrontBuffer->get_buffer();
				_pCurrentWriteBase = _pBackBuffer ->get_buffer();

				_pFrontBuffer->reset();
				_pFrontQueue ->reset();
			}

#pragma endregion

#pragma region memory reporting

			unsigned short front_buffer_size() const { return _pFrontBuffer->buffer_size(); }
			unsigned short front_buffer_free() const { return _pFrontBuffer->free(); }

			unsigned short back_buffer_size() const { return _pBackBuffer->buffer_size(); }
			unsigned short back_buffer_free() const { return _pBackBuffer->free(); }

			size_t front_queue_size() const { return _pFrontQueue->buffer_size(); }
			size_t front_queue_free() const { return _pFrontQueue->free(); }

			size_t back_queue_size() const { return _pBackQueue->buffer_size(); }
			size_t back_queue_free() const { return _pBackQueue->free(); }

#pragma endregion

			const std::string& Name() const { return _name; }

		private:

			std::string                  _name;

			std::unique_ptr<queue_type>  _pFrontQueue;
			std::unique_ptr<queue_type>  _pBackQueue;

			std::unique_ptr<MCMessageQueueDataBuffer> _pFrontBuffer;
			std::unique_ptr<MCMessageQueueDataBuffer> _pBackBuffer;

			char* _pCurrentReadBase;
			char* _pCurrentWriteBase;
	};
}
