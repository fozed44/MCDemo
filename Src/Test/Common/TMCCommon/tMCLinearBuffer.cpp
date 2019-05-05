#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include "../../../Common/MCCommon/src/Data/MCLinearBuffer.h"
#define MCMANAGEDHANDLE_UNIT_TEST_HELPERS

#include "../../../Common/MCCommon/src/Headers/MCManagedHandle.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon
{
	class LinearBufferTestType {
	public:
		LinearBufferTestType(int a, int b) {
			A = a;
			B = b;
			TotalAllocations++;
		}
		LinearBufferTestType(const LinearBufferTestType& o) {
			A = o.A;
			B = o.B;
			TotalAllocations++;
		}
		LinearBufferTestType& operator= (const LinearBufferTestType& o) {
			A = o.A;
			B = o.B;
			TotalAllocations++;
			return *this;
		}
		~LinearBufferTestType() {
			TotalAllocations--;
		}
		int A;
		int B;
	public:
		static void ResetAllocations() {
			TotalAllocations.store(0);
		}
		static unsigned int GetTotalAllocations() {
			return TotalAllocations.load();
		}
	private:
		static std::atomic<unsigned int> TotalAllocations;
	};

	std::atomic<unsigned int> LinearBufferTestType::TotalAllocations;

	TEST_CLASS(TestMCLinearBuffer)
	{
		TEST_METHOD(TestMCLeanearBuffer_Instantiate)
		{
			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());

			MCLinearBuffer<LinearBufferTestType, 5> testBuffer;

			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());
		}

		TEST_METHOD(TestMCLeanearBuffer_Instantiate_Destroy)
		{
			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());

			MCLinearBuffer<LinearBufferTestType, 5> testBuffer;

			testBuffer.add({ 1, 1 });
			testBuffer.add({ 2, 2 });
			testBuffer.add({ 3, 3 });
			testBuffer.add({ 4, 4 });
			testBuffer.add({ 5, 5 });

			Assert::AreEqual(5u, LinearBufferTestType::GetTotalAllocations());

			testBuffer.clear(4);
			testBuffer.clear(3);
			testBuffer.clear(2);
			testBuffer.clear(1);
			testBuffer.clear(0);

			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());
		}

		TEST_METHOD(TestMCLinearBuffer_Instantiate_Fail_When_Full)
		{
			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());

			MCLinearBuffer<LinearBufferTestType, 1> testBuffer;
			int pos;

			pos = testBuffer.add({ 1,1 });

			Assert::AreEqual(1u, LinearBufferTestType::GetTotalAllocations());
			Assert::AreEqual(0, pos);

			pos = testBuffer.add({ 2,2 });

			Assert::AreEqual(1u, LinearBufferTestType::GetTotalAllocations());
			Assert::AreEqual(-1, pos);

			testBuffer.clear(0);

			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());

			pos = testBuffer.add({ 1,1 });

			Assert::AreEqual(1u, LinearBufferTestType::GetTotalAllocations());
			Assert::AreEqual(0, pos);

			pos = testBuffer.add({ 2,2 });

			Assert::AreEqual(1u, LinearBufferTestType::GetTotalAllocations());
			Assert::AreEqual(-1, pos);
		}

		TEST_METHOD(TestMCLinearBuffer_DestroyOnDestructor)
		{
			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());

			{
				MCLinearBuffer<LinearBufferTestType, 5> testBuffer;

				testBuffer.add({ 1, 1 });
				testBuffer.add({ 2, 2 });
				testBuffer.add({ 3, 3 });
				testBuffer.add({ 4, 4 });
				testBuffer.add({ 5, 5 });

				Assert::AreEqual(5u, LinearBufferTestType::GetTotalAllocations());
			}

			Assert::AreEqual(0u, LinearBufferTestType::GetTotalAllocations());
		}
	};
}