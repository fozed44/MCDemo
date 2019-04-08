#pragma once

#include "MCD3D12Core.h"

namespace MC {

	class MCMath {
	public:
		// Get a random float [0,1)
		static float RandF() {
			return (float)(rand()) / (float)RAND_MAX;
		}

		// Get a random float [a,b)
		static float RandF(float a, float b) {
			return a + RandF()*(b - a);
		}

		// Get a random int [a,b)
		static int Rand(int a, int b) {
			return a + rand() % ((b - a) + 1);
		}


		template<typename T>
		static T Min(const T& a, const T& b) {
			if (a < b)
				return a;
			return b;
		}

		template<typename T>
		static T Max(const T& a, const T& b) {
			if (a > b)
				return a;
			return b;
		}

		template<typename T>
		static T Lerp(const T& a, const T& b, float t) {
			return a + ((b - a) * t)
		}

		static DirectX::XMFLOAT4X4 Identity4x4() {
			static DirectX::XMFLOAT4X4 i(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			);

			return i;
		}

		template<typename T>
		static T Abs(const T& a) {
			if (a >= 0)
				return a;
			return -a;
		}

	};

}