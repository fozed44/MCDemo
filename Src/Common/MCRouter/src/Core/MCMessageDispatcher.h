//#pragma once
//
//#include "MCMessageQueue.h"
//
//namespace MC {
//
//	template<
//		typename tMessage,
//		typename tRouter,
//		void (tRouter::*fMessage)(const tMessage*),
//		int N1,
//	    int N2>
//		class MCMessageDispatcher {
//			using queue_type = MCMessageQueue<tMessage, N1, N2>;
//		public:
//			MCMessageDispatcher(queue_type *pQueue) {
//				_pQueue = pQueue;
//			}
//			~MCMessageDispatcher() {}
//			MCMessageDispatcher(MCMessageDispatcher&)              = delete;
//			MCMessageDispatcher(MCMessageDispatcher&&)             = delete;
//			MCMessageDispatcher& operator= (MCMessageDispatcher&)  = delete;
//			MCMessageDispatcher& operator= (MCMessageDispatcher&&) = delete;
//			
//		public:
//			void run_dispatch() {
//				_pQueue->add_read_lock();
//				tMessage msg;
//				while (_pQueue->pop(&msg)) {
//
//				}
//
//				_pQueue->release_read_lock();
//			}
//		private:
//			queue_type* _pQueue;
//	};
//
//	template<
//		typename tMessage,
//		typename tRouter,
//		void (tRouter::*fMessage)(const tMessage*),
//		void (tRouter::*fAddressedMessage)(const tMessage*, const char*),
//		int N1,
//		int N2>
//		class MCAddressableMessageDispatcher {
//		using queue_type = MCMessageQueue<tMessage, N1, N2>;
//		public:
//			MCAddressableMessageDispatcher(queue_type *pQueue) {
//				_pQueue = pQueue;
//			}
//			~MCAddressableMessageDispatcher() {}
//			MCAddressableMessageDispatcher(MCAddressableMessageDispatcher&)              = delete;
//			MCAddressableMessageDispatcher(MCAddressableMessageDispatcher&&)             = delete;
//			MCAddressableMessageDispatcher& operator= (MCAddressableMessageDispatcher&)  = delete;
//			MCAddressableMessageDispatcher& operator= (MCAddressableMessageDispatcher&&) = delete;
//
//		public:
//			void run_dispatch() {
//				_pQueue->add_read_lock();
//				tMessage msg;
//				while (_pQueue->pop(&msg)) {
//
//				}
//
//				_pQueue->release_read_lock();
//			}
//		private:
//			queue_type * _pQueue;
//	};
//
//}
