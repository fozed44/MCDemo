#pragma once

#include "../Data/MCLinearBuffer.h"
#include "MCIAnalyzer.h"

namespace MC {

	template <typename tBuffer>
	class MCLinearBufferAnalyzer : public MCIAnalyzer {
	public: /* Types */
		using BufferType = tBuffer;
	public: /* ctor */
		MCLinearBufferAnalyzer(BufferType* pBuffer) : _pBuffer{ pBuffer } {}
		virtual ~MCLinearBufferAnalyzer() {}
		MCLinearBufferAnalyzer(MCLinearBufferAnalyzer&)              = delete;
		MCLinearBufferAnalyzer(MCLinearBufferAnalyzer&&)             = delete;
		MCLinearBufferAnalyzer& operator= (MCLinearBufferAnalyzer&)  = delete;
		MCLinearBufferAnalyzer& operator= (MCLinearBufferAnalyzer&&) = delete;

	public: /* MCIAnalyzer */
		std::string Analyze() { return CountActiveObjects(); }
	public:
		std::string CountActiveObjects() {
			unsigned int bufferSize = _pBuffer->size();
			unsigned int activeObjects = 0;
			for (unsigned int x = 0; x < bufferSize; x++) {
				if (_pBuffer->_pBuffer[x].Flags == BufferType::MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED)
					activeObjects++;
			}

			return std::string("There are ") + std::to_string(activeObjects) + std::string(" active objects in the buffer.");
		}
	private:
		BufferType* _pBuffer;
	};

}

