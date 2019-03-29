#pragma once

namespace MC {

	/*
		These primitives are strictly to abstract non-rendering code from rendering code.

		These primitives are do not have the fast SIMD code that is supplied for rendering primitives, therefore
		these primitives should only be used when necessary.
	*/

	struct MCFLOAT2 {
		float x;
		float y;
	};

	struct MCFLOAT3 {
		float x;
		float y;
		float z;
	};

	struct MCFLOAT4 {
		float x;
		float y;
		float z;
		float w;
	};

	template <typename T_>
	class AllocBuffer {
	private:
		enum ALLOC_BUFFER_ELEMENT_FLAGS {
			ALLOC_BUFFER_ELEMENT_FLAGS_FREE      = 0,
			ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED = 1
		};
		struct AllocBufferElement {
			union {
				ALLOC_BUFFER_ELEMENT_FLAGS Flags;
				char *pPadding;
			};
			T_ Payload;
		};
	public:
		AllocBuffer(size_t capacity)
			: _pBuffer{ nullptr },
			  _bufferSize{ 0 },
			  _emptyCount{ capacity } {
			realloc(capacity);
		}
		AllocBuffer() : AllocBuffer(10) { }
		~AllocBuffer() {
			if (pBuffer) {
				for (int x = 0; x < _bufferSize / sizeof(AllocBufferElement); ++x)
					if (_pBuffer[x].Flags & ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED)
						pBuffer[x].Payload.~T_();
			}
		}
		AllocBuffer(AllocBuffer&) = delete;
		AllocBuffer(AllocBuffer&&) = delete;
		AllocBuffer& operator= (AllocBuffer&) = delete;
		AllocBuffer& operator= (AllocBuffer&&) = delete;
		unsigned int add(const T_& a) {
			unsigned int pos = getNextEmptyElement();
			AllocBufferElement* ptr = &pBuffer[pos];
			ptr.Flags |= ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED;
			ptr.Payload = a;

			_emptyCount--;

			return pos;
		}
		void clear(size_t index) {
			assert(index < _bufferElementSize);

			AllocBufferElement* ptr = &pBuffer[index];

			if (!(ptr->Flags & ALLOC_BUFFER_ELEMENT_FLAGS_ALOCATED))
				return;

			ptr->Flags = ALLOC_BUFFER_ELEMENT_FLAGS_FREE;
			ptr->Payload = {};

			_emptyCount++;
		}
		const T_* get(unsigned int index) const {
			assert(index < _bufferElementSize);

			AllocBufferElement* ptr = &_pBuffer[index];

			assert(ptr->Flags & ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED);

			return &ptr->Payload;
		}

		T_* get(unsigned int index) {
			assert(index < _bufferElementSize);

			AllocBufferElement* ptr = &_pBuffer[index];

			assert(ptr->Flags & ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED);

			return &ptr->Payload;
		}
	private:
		size_t getNextEmptyElement() {
			for (size_t x = 0; x < _bufferElementSize; ++x)
				if (!(_pBuffer[x].Flags & ALLOC_BUFFER_ELEMENT_FLAGS_ALLOCATED))
					return x;
		}
		void realloc(size_t newSize) {
			assert(newSize > _bufferByteSize);
			char* newBuffer = malloc(newSize);
			memcpy(newBuffer, _pBuffer, _bufferByteSize);
			_pBuffer = newBuffer;
			_bufferByteSize = newSize;
		}
		AllocBufferElement *_pBuffer;
		size_t _bufferByteSize;
		size_t _bufferElementSize;
		size_t _emptyCount;
	};

}
