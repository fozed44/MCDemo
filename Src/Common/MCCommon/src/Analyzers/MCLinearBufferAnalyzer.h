#pragma once

#include "../Data/MCLinearBuffer.h"
#include "MCIAnalyzer.h"
#include <sstream>
#include <iomanip>

namespace MC {	

	template <typename tBufferElement, int bufferSize>
	class MCLinearBufferAnalyzer : public MCIAnalyzer {
	private: /* Analysis data. */
		typedef struct MCLinearBufferSummary {
			unsigned totalElements;
			unsigned usedElements;
			unsigned totalBytes;
			unsigned usedBytes;
			float    pctElements;
			float    pctBytes;
		} MCLinearBufferrSummary;
	public: /* Types */
		using BufferType = MCLinearBuffer<tBufferElement, bufferSize>;
	public: /* ctor */
		MCLinearBufferAnalyzer(BufferType* pBuffer) : _pBuffer{ pBuffer } {}
		virtual ~MCLinearBufferAnalyzer() {}
		MCLinearBufferAnalyzer(MCLinearBufferAnalyzer&)              = delete;
		MCLinearBufferAnalyzer(MCLinearBufferAnalyzer&&)             = delete;
		MCLinearBufferAnalyzer& operator= (MCLinearBufferAnalyzer&)  = delete;
		MCLinearBufferAnalyzer& operator= (MCLinearBufferAnalyzer&&) = delete;

	public: /* MCIAnalyzer */
		std::string Summary() { return GenerateSummary(); }
		std::string Detail(std::function<std::string(tBufferElement buffer, MCLinearBufferAddress address)> describeItem)  { return GenerateDetail(describeItem); }
		std::string Detail() { std::string("Use Detail(func)!"); }
	public:
		unsigned count_used_elements() {
			unsigned bufferSize    = _pBuffer->size();
			unsigned usedElements = 0;
			for (unsigned int x = 0; x < bufferSize; x++) {
				if (_pBuffer->_pBuffer[x].Flags == BufferType::MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED)
					usedElements++;
			}
			return usedElements;
		}
		unsigned count_total_elements() {
			return _pBuffer->size();
		}
		unsigned count_used_bytes() {
			return count_used_elements() * _pBuffer->element_byte_size();
		}
		unsigned count_total_bytes() {
			return _pBuffer->size() * _pBuffer->element_byte_size();
		}
		MCLinearBufferSummary InternalSummary() {
			MCLinearBufferAnalysis result;
			result.totalElements = count_total_elements();
			result.usedElements  = count_used_elements();
			result.totalBytes    = count_total_bytes();
			result.usedBytes     = count_used_bytes();
			result.pctElements   = static_cast<float>(result.usedElements) / static_cast<float>(result.totalElements);
			result.pctBytes      = static_cast<float>(result.usedBytes)    / static_cast<float>(result.totalBytes);
			return result;
		}
		std::string GenerateSummary() {
			MCLinearBufferSummary analysis = InternalSummary();
			std::ostringstream elements; std::ostringstream bytes;
			elements << analysis.usedElements << "/" << analysis.totalElements;
			bytes    << analysis.usedBytes    << "/" << analysis.totalBytes;
			std::ostringstream o;
			o << "\n"
			  << std::left << std::setw(20) << "Items Used/Total" << ":" << std::right << std::setw(10) << elements.str() << std::right << std::setw(8) << analysis.pctElements << "%" << "\n"
			  << std::left << std::setw(20) << "Bytes Used/Total" << ":" << std::right << std::setw(10) << bytes.str()    << std::right << std::setw(8) << analysis.pctBytes    << "%" << std::endl;
			return o.str();
		}		
		std::string GenerateDetail(std::function<std::string(tBufferElement buffem, MCLinearBufferAddress address)> describeItem) {
			return
				GenerateDetailForAllBufferItems(describeItem)
			  + GenerateSummary();
		}
		std::string GenerateDetailForAllBufferItems(std::function<std::string(tBufferElement element, MCLinearBufferAddress address)> describeItem) {
			std::ostringstream o;			
			for (MCLinearBufferAddress address : _pBuffer->enumerate_all_used_addresses())
				o << "\n" << describeItem(_pBuffer->get(address), address);
			return o.str();
		}
	private:
		BufferType* _pBuffer;
	};

}

