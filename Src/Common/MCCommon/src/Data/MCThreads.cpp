#include "MCThreads.h"
#include "assert.h"
namespace MC {

	std::map<std::thread::id, std::tuple<std::string, MC_THREAD_CLASS>> MCThreads::_threadMap;

	void MCThreads::RegisterCurrentThread(const std::string& name, MC_THREAD_CLASS threadClass) {
		static_assert(sizeof(std::thread::id) == sizeof(uint32_t), "this function only works if size of thead::id is equal to the size of uint_64");
		

		auto thisThreadId = std::this_thread::get_id();

		auto current = _threadMap.find(thisThreadId);

		assert(current == _threadMap.end());

		_threadMap[thisThreadId] = { name, threadClass };
	}

	void MCThreads::Assert(const std::string& name) {
		auto thisThreadId = std::this_thread::get_id();

		auto current = _threadMap.find(thisThreadId);

		assert(current != _threadMap.end());
		assert(std::get<0>(current->second) == name);
	}

	void MCThreads::Assert(MC_THREAD_CLASS threadClass) {
		auto thisThreadId = std::this_thread::get_id();

		auto current = _threadMap.find(thisThreadId);

		assert(current != _threadMap.end());
		assert(std::get<1>(current->second) == threadClass);
	}

	int MCThreads::ID() {
		auto id = std::this_thread::get_id();
		uint32_t* ptr = (uint32_t*)&id;
		return (*ptr);
	}

	std::string MCThreads::Name() {
		auto thisThreadId = std::this_thread::get_id();

		auto current = _threadMap.find(thisThreadId);

		assert(current != _threadMap.end());

		return std::get<0>(current->second);
	}

	MC_THREAD_CLASS MCThreads::ThreadClass() {
		auto thisThreadId = std::this_thread::get_id();

		auto current = _threadMap.find(thisThreadId);

		assert(current != _threadMap.end());

		return std::get<1>(current->second);
	}
}