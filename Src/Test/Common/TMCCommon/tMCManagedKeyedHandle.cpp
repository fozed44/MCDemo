#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>
#define MCMANAGEDHANDLE_UNIT_TEST_HELPERS

#include "../../../Common/MCCommon/src/Headers/MCManagedKeyedHandle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MC;
namespace TMCCommon
{

	struct Person {
		Person(int t1, int t2) { testProp1 = t1, testProp2 = t2; }
		~Person() {
			testProp2 = testProp1;
		}
		int testProp1;
		int testProp2;
	};

	struct PersonHandle {
		Person* ptr;
		__int64 fence;
		Person* Key() { return ptr; }
	};

	class ManagedKeyedPersonManager : public MCManagedKeyedHandleManager<PersonHandle, Person*, std::shared_ptr<Person>, ManagedKeyedPersonManager> {
	public:
		~ManagedKeyedPersonManager() {}
		ManagedKeyedPersonManager(ManagedKeyedPersonManager&) = delete;
		ManagedKeyedPersonManager& operator= (ManagedKeyedPersonManager&) = delete;
		ManagedKeyedPersonManager& operator= (ManagedKeyedPersonManager&&) = delete;
	private:
		ManagedKeyedPersonManager() {}
	public:
		inline static ManagedKeyedPersonManager* Instance() {
			static ManagedKeyedPersonManager instance;
			return &instance;
		}
		inline ManagedKeyedPersonManager::tManagedKeyedHandle NewHandle(std::shared_ptr<Person>& p) {
			return CreateRef(PersonHandle{ p.get(), 0 }, p);
		}
	};


	TEST_CLASS(TestMCManagedHandle)
	{
	public:

		TEST_METHOD(ManagedHandleManager_RefCount)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
				// No handles have been created; there the reference count should be 0.
				Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				// We have one handle for testPerson, the reference count should be 1.
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			{
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle2;
				// Test handle has gone out of scope, we should be back to 0.
				Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle2 = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				// We have created another handle for testPerson, reference count should be 1.
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			ManagedKeyedPersonManager::tManagedKeyedHandle testHandle3;
			// testHandle1 and testHandle2 have been destructed, we should once again be at 0.
			Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				// This test will create 2 handles for the same test person.
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle2;
				// NO handles have been created yet, so the reference count should be at 0.
				Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				// After the first handle has been added, we should have a single reference.
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle2 = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				// Now there are two handles to the test Person, so we should have a reference count of 2.
				Assert::AreEqual(2, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			// Both handles have gone out of scope, so we should have 0 references.
			Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_stagered)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
				{
					// This test will create 2 handles for the same test person.
					// But one handle exists in the outer scope while one exists in the inner scope.
					ManagedKeyedPersonManager::tManagedKeyedHandle testHandle2;

					// NO handles have been created yet, so the reference count should be at 0.
					Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));

					testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
					// After the first handle has been added, we should have a single reference.
					Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));

					testHandle2 = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
					// Now there are two handles to the test Person, so we should have a reference count of 2.
					Assert::AreEqual(2, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
				}
				// TestHandle2 has gone out of scope but testHandle remains valid, so there should be a 
				// reference count of 1.
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			// Both handles have gone out of scope, so we should have 0 references.
			Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_StaySingle)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				// Calling NewHandle (which called MCManagedHandleManager.CreateRef) and storing the result in
				// the SAME handle should keep the reference count at 1.
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
				Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
			Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_stagered_threaded)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			std::vector<std::thread> threads;
			{
				ManagedKeyedPersonManager::tManagedKeyedHandle testHandle;
				for (int x = 0; x < 1000; x++) {
					threads.push_back(std::thread([&]()
					{
						{
							ManagedKeyedPersonManager::tManagedKeyedHandle testHandle2;
							{
								// This test will create 2 handles for the same test person.
								// But one handle exists in the outer scope while one exists in the inner scope.
								ManagedKeyedPersonManager::tManagedKeyedHandle testHandle3;
								testHandle = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
								testHandle2 = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
								testHandle3 = ManagedKeyedPersonManager::Instance()->NewHandle(testPerson);
							}
						}
					}
					));
				}
				for (int x = 0; x < 1000; x++)
					threads[x].join();
				Assert::AreEqual(1, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
			}

			Assert::AreEqual(0, ManagedKeyedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

	};
}