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

#ifdef __DEBUG_MCMESSAGE_QUEUE__
			_writeOnly = true;
#endif __DEBUG_MCMESSAGE_QUEUE__

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
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + Pad32(sizeof(T)) < _pEnd));
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			return reinterpret_cast<T*>(ptr);
		}

		char* push_to(unsigned short size) {
			size = (size-1) / 4 * 4 + 4; // pad size to 32 bits. 
			char* ptr = _pNextPush.fetch_add(size);
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT((ptr + size) < _pEnd);
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
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

#ifdef __DEBUG_MCMESSAGE_QUEUE__
		void AllowWrite(bool allowWrite) { 
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN); 
			_writeOnly = allowWrite;
		}
#endif __DEBUG_MCMESSAGE_QUEUE__

		void reset() {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);
			assert(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			_pNextPush.store(_pBuffer);
		}

		char* allocate(unsigned short dataSize) {
			dataSize = (dataSize - 1) / 4 * 4 + 4; // pad size to 32 bits. 
			char* ptr = _pNextPush.fetch_add(dataSize);
#ifdef __DEBUG_MCMESSAGE_QUEUE__
			MCASSERT(_pNextPush <= _pBuffer);
			MCASSERT(_writeOnly);
#endif __DEBUG_MCMESSAGE_QUEUE__
			return ptr;
		}

	private:
		std::string        _name;
		char*              _pBuffer;
		char*              _pEnd;
		std::atomic<char*> _pNextPush;
#ifdef __DEBUG_MCMESSAGE_QUEUE__
		bool               _writeOnly;
#endif __DEBUG_MCMESSAGE_QUEUE__
	};

	/*******************************************************************************************/

	template<
		typename tMessage, /* The core type being queued. */
		unsigned int N1,   /* The size of the queues. */
		unsigned int N2	   /* The size of the memory buffers. */
	>
		class MCMessageQueue {
		using queue_type  = MCLinearFIFO<tMessage, N1>;
		public:
			MCMessageQueue(const std::string& name) : _name{ name } {
				MCCriticalSection::InitializeLock(&_lock);
				_pFrontQueue = std::make_unique<queue_type>();
				_pBackQueue  = std::make_unique<queue_type>();

				_pFrontBuffer = std::make_unique<MCMessageQueueDataBuffer>(_name + std::string(" buffer a"), N2);
				_pBackBuffer  = std::make_unique<MCMessageQueueDataBuffer>(_name + std::string(" buffer b"), N2);

				_pCurrentReadBase  = _pFrontBuffer->get_buffer();
				_pCurrentWriteBase = _pBackBuffer->get_buffer();
			}
			~MCMessageQueue() {}
			MCMessageQueue(MCMessageQueue&)              = delete;
			MCMessageQueue(MCMessageQueue&&)             = delete;
			MCMessageQueue& operator= (MCMessageQueue&)  = delete;
			MCMessageQueue& operator= (MCMessageQueue&&) = delete;
		public: /* push, push_to */

			/*	Push a normal message. (A message that does not use an external Address) */
			void push(const tMessage& message) {
				_pFrontQueue->push(message);
			}

			char* push_to(const tMessage& message, unsigned short dataSize) {
				char*     ptr = _pFrontBuffer->push_to(dataSize);
				tMessage* msg = _pFrontQueue ->push_to();
				*msg = message;
				msg->Address = ptr;
				return ptr;
			}

			template<typename T>
			T* push_to(const tMessage& t) {
				char*     ptr = _pFrontBuffer->push_to(sizeof(T));
				tMessage* msg = _pFrontQueue ->push_to();
				*msg = t;
				msg->Address = ptr;
				return reinterpret_cast<T*>(ptr);
			}

		public: /* Arbitrary storage */

			char* allocate(unsigned short dataSize) {
				_pFrontBuffer->allocate(unsigned short dataSize);
			}

		public: /* pop, peek */
			bool pop(tMessage* pMessage) {
				return _pBackQueue->pop(pMessage);
			}

			const tMessage& peek() const {
				return _pBackQueue->peek();
			}

#pragma region read/write locks

		public: /* concurrency management */

			void add_read_lock() {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
				MCThreads::AssertRegistered();

				if (std::find(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()) != _readLocks.end()) {
					MCTHROW("MCMessageQueue::add_read_lock->threadId already exists.");
				}
#endif __DEBUG_MCMESSAGE_QUEUE__
				ENTER_CRITICAL_SECTION(MCMessageQueue_add_read_lock, &_lock);
					_readLocks.push_back(std::this_thread::get_id());
				EXIT_CRITICAL_SECTION;
			}

			void release_read_lock() {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
				MCThreads::AssertRegistered();

				if (std::find(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()) == _readLocks.end()) {
					MCTHROW("MCMessageQueue::release_read_lock->threadId does not exist.");
				}

				size_t readLocksOldCount = _readLocks.size();

#endif __DEBUG_MCMESSAGE_QUEUE__
				ENTER_CRITICAL_SECTION(MCMessageQueue_release_read_lock, &_lock);
					_readLocks.erase(std::remove(_readLocks.begin(), _readLocks.end(), std::this_thread::get_id()), _readLocks.end());
				EXIT_CRITICAL_SECTION;

#ifdef __DEBUG_MCMESSAGE_QUEUE__				
				assert(readLocksOldCount - 1 == _readLocks.size());
#endif __DEBUG_MCMESSAGE_QUEUE__
			}

			void add_write_lock() {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
				MCThreads::AssertRegistered();

				if (std::find(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()) != _writeLocks.end()) {
					MCTHROW("MCMessageQueue::add_write_lock->threadId already exists.");
				}
#endif __DEBUG_MCMESSAGE_QUEUE__
				ENTER_CRITICAL_SECTION(MCMessageQueue_add_read_lock, &_lock);
				_writeLocks.push_back(std::this_thread::get_id());
				EXIT_CRITICAL_SECTION;
			}

			void release_write_lock() {
#ifdef __DEBUG_MCMESSAGE_QUEUE__
				MCThreads::AssertRegistered();

				if (std::find(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()) == _writeLocks.end()) {
					MCTHROW("MCMessageQueue::release_write_lock->threadId does not exist.");
				}

				size_t readLocksOldCount = _writeLocks.size();

#endif __DEBUG_MCMESSAGE_QUEUE__
				ENTER_CRITICAL_SECTION(MCMessageQueue_release_read_lock, &_lock);
				_writeLocks.erase(std::remove(_writeLocks.begin(), _writeLocks.end(), std::this_thread::get_id()), _writeLocks.end());
				EXIT_CRITICAL_SECTION;

#ifdef __DEBUG_MCMESSAGE_QUEUE__				
				assert(readLocksOldCount - 1 == _writeLocks.size());
#endif __DEBUG_MCMESSAGE_QUEUE__
			}

#pragma endregion

#pragma region swap

			MC_RESULT swap() {
				MCTHREAD_ASSERT(MC_THREAD_CLASS_MAIN);

					if (_readLocks.size() || _writeLocks.size())
						return MC_RESULT_FAIL_RESOURCE_LOCKED;

				ENTER_CRITICAL_SECTION(MCMessageQueue_swap, &_lock);

					if (_readLocks.size() || _writeLocks.size())
						return MC_RESULT_FAIL_RESOURCE_LOCKED;

#ifdef __DEBUG_MCMESSAGE_QUEUE__
					_pFrontBuffer->AllowWrite (false);
					_pFrontQueue ->allow_write(false);

					_pBackBuffer->AllowWrite (true);
					_pBackQueue ->allow_write(true);
#endif __DEBUG_MCMESSAGE_QUEUE__

					_pFrontBuffer.swap(_pBackBuffer);
					_pFrontQueue .swap(_pBackQueue);

					_pCurrentReadBase  = _pFrontBuffer->get_buffer();
					_pCurrentWriteBase = _pBackBuffer ->get_buffer();

					_pFrontBuffer->reset();
					_pFrontQueue ->reset();

				EXIT_CRITICAL_SECTION;
			}

#pragma endregion

#pragma region memory reporting

			unsigned short front_buffer_size() const { return _pFrontBuffer->buffer_size(); }
			unsigned short front_buffer_free() const { return _pFrontBuffer->free(); }

			unsigned short back_buffer_size() const { return _pBackBuffer->buffer_size(); }
			unsigned short back_buffer_free() const { return _pBackBuffer->free(); }

#pragma endregion

			const std::string& Name() const { return _name; }

		private:

			std::string                  _name;

			std::unique_ptr<queue_type>  _pFrontQueue;
			std::unique_ptr<queue_type>  _pBackQueue;

			std::unique_ptr<MCMessageQueueDataBuffer> _pFrontBuffer;
			std::unique_ptr<MCMessageQueueDataBuffer> _pBackBuffer;

			char*                        _pCurrentReadBase;
			char*                        _pCurrentWriteBase;

			MCCriticalSectionLock        _lock;
			std::vector<std::thread::id> _writeLocks;
			std::vector<std::thread::id> _readLocks;
	};

}
