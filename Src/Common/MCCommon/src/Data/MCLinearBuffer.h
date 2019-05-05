#pragma once

#include "stdlib.h"
#include "stdio.h"
#include "assert.h"
#include "shared_mutex"

namespace MC {
		
	typedef int MCLinearBufferAddress;

	template <typename T_, unsigned int N>
	class MCLinearBuffer {
	private:
		enum MC_LINEAR_BUFFER_ELEMENT_FLAGS : unsigned int {
			MC_LINEAR_BUFFER_ELEMENT_FLAGS_FREE      = 0,
			MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED = 1
		};
		struct MCLinearBufferElement {
			unsigned int Flags;
			T_           Payload; 
		};
	public:
		MCLinearBuffer()
			: _pBuffer{ nullptr },
			  _emptyCount{ N } {
			_pBuffer = static_cast<MCLinearBufferElement*>(malloc(sizeof(MCLinearBufferElement) * N));
			memset(_pBuffer, 0, sizeof(MCLinearBufferElement) * N);
		}
		
		~MCLinearBuffer() {
			if (_pBuffer) {
				for (int x = 0; x < N; ++x)
					if (_pBuffer[x].Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED)
						_pBuffer[x].Payload.~T_();
				free(_pBuffer);
			}
		}
		MCLinearBuffer(MCLinearBuffer&)              = delete;
		MCLinearBuffer(MCLinearBuffer&&)             = delete;
		MCLinearBuffer& operator= (MCLinearBuffer&)  = delete;
		MCLinearBuffer& operator= (MCLinearBuffer&&) = delete;
		unsigned int freeSpace() {
			return _emptyCount;
		}
		MCLinearBufferAddress add(const T_& a) {			
			if (!_emptyCount)
				return MCLinearBuffer::InvalidAddress;
				
			MCLinearBufferAddress pos = getNextEmptyElement();

			if (pos < 0)
				return pos;
			
			MCLinearBufferElement* ptr = &_pBuffer[pos];
			
			ptr->Flags  |= MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED;
			ptr->Payload = a;

			_emptyCount--;

			assert(_emptyCount >= 0); // Ensure that _emptyCount >= 0 multi-threading
									  // could cause an issue if the calling code is 
									  // not careful.

			return pos;
		}
		void set(MCLinearBufferAddress index, const T_& a) {
			assert(index >= 0 && index < N);
			MCLinearBufferElement* ptr = &_pBuffer[index];
			
			if (!(ptr->Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED)) {
				_emptyCount--;

				assert(_emptyCount >= 0); // Ensure that _emptyCount >= 0 multi-threading
										  // could cause an issue if the calling code is 
										  // not careful.

				ptr->Flags |= MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED;
			}
			ptr->Payload = a;
		}
		void clear(MCLinearBufferAddress index) {
			assert(index >= 0 && index < N);

			MCLinearBufferElement* ptr = &_pBuffer[index];

			if (!(ptr->Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED))
				return;

			ptr->Payload.~T_();
			ptr->Flags = MC_LINEAR_BUFFER_ELEMENT_FLAGS_FREE;

#ifdef _DEBUG
			char* chPtr = reinterpret_cast<char*>(ptr);
			chPtr += sizeof(MCLinearBufferElement::Flags);
			memset(chPtr, 0xAB, (sizeof(MCLinearBufferElement) - sizeof(MCLinearBufferElement::Flags)));
#endif _DEBUG

			_emptyCount++;
		}
		void clear() {
			for (MCLinearBufferAddress x = 0; x < N; ++x) {
				if (_pBuffer[x].Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED) {
					MCLinearBufferElement* ptr = &_pBuffer[index];

					ptr->Payload.~T_();
					ptr->Flags = MC_LINEAR_BUFFER_ELEMENT_FLAGS_FREE;

#ifdef _DEBUG
					char* chPtr = reinterpret_cast<char*>(ptr);
					chPtr += sizeof(MCLinearBufferElement::Flags);
					memset(chPtr, 0xAB, (sizeof(MCLinearBufferElement) - sizeof(MCLinearBufferElement::Flags)));
#endif _DEBUG
				}
			}
			_emptyCount = N;
		}
		const T_* get(MCLinearBufferAddress index) const {
			assert(index >= 0 && index < N);

			MCLinearBufferElement* ptr = &_pBuffer[index];

			assert(ptr->Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED);

			return &ptr->Payload;
		}

		T_* get(MCLinearBufferAddress index) {
			assert(index >= 0 && index < N);

			MCLinearBufferElement* ptr = &_pBuffer[index];

			assert(ptr->Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED);

			return &ptr->Payload;
		}

		MCLinearBufferAddress find(const T_& obj) {
			MCLinearBufferElement *pCurrent;
			for (MCLinearBufferAddress x = 0; x < N; ++x) {
				pCurrent = &_pBuffer[x];
				if (pCurrent->Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED
			     && pCurrent->Payload == obj)
					return x;
			}
			return MCLinearBuffer::InvalidAddress;
		}

		static const MCLinearBufferAddress InvalidAddress = -1;
	private:
		MCLinearBufferAddress getNextEmptyElement() {
			for (MCLinearBufferAddress x = 0; x < N; ++x)
				if (!(_pBuffer[x].Flags & MC_LINEAR_BUFFER_ELEMENT_FLAGS_ALLOCATED))
					return x;
			return MCLinearBuffer::InvalidAddress;
		}
		MCLinearBufferElement *_pBuffer;
		size_t                 _emptyCount;	
	};

}