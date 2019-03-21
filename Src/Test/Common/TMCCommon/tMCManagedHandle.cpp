#include "stdafx.h"
#include "CppUnitTest.h"
#include <memory>
#include <thread>
#include <vector>
#define MCMANAGEDHANDLE_UNIT_TEST_HELPERS

#include "../../../Common/MCCommon/src/Headers/MCManagedHandle.h"

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

	class ManagedPersonManager;
	typedef MCManagedHandle<Person*, ManagedPersonManager> ManagedPersonHandle;

	class ManagedPersonManager : public MCManagedHandleManager<Person*, std::shared_ptr<Person>, ManagedPersonManager> {
	public:
		~ManagedPersonManager() {}
		ManagedPersonManager(ManagedPersonManager&) = delete;
		ManagedPersonManager& operator= (ManagedPersonManager&) = delete;
		ManagedPersonManager& operator= (ManagedPersonManager&&) = delete;
	private:
		ManagedPersonManager(){}
	public:
		inline static ManagedPersonManager* Instance() {
			static ManagedPersonManager instance;
			return &instance;
		}
		inline ManagedPersonHandle NewHandle(std::shared_ptr<Person>& p) {
			return CreateRef(p.get(), p);
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
				ManagedPersonHandle testHandle;
				// No handles have been created; there the reference count should be 0.
				Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
				// We have one handle for testPerson, the reference count should be 1.
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}			
			{
				ManagedPersonHandle testHandle2;
				// Test handle has gone out of scope, we should be back to 0.
				Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle2 = ManagedPersonManager::Instance()->NewHandle(testPerson);
				// We have created another handle for testPerson, reference count should be 1.
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			ManagedPersonHandle testHandle3;
			// testHandle1 and testHandle2 have been destructed, we should once again be at 0.
			Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount)
		{
			auto testPerson = std::make_shared<Person>(1, 1);		
			auto pTestPerson = testPerson.get();
			{
				// This test will create 2 handles for the same test person.
				ManagedPersonHandle testHandle;
				ManagedPersonHandle testHandle2;
				// NO handles have been created yet, so the reference count should be at 0.
				Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
				// After the first handle has been added, we should have a single reference.
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));

				testHandle2 = ManagedPersonManager::Instance()->NewHandle(testPerson);
				// Now there are two handles to the test Person, so we should have a reference count of 2.
				Assert::AreEqual(2, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			// Both handles have gone out of scope, so we should have 0 references.
			Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_stagered)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				ManagedPersonHandle testHandle;
				{
					// This test will create 2 handles for the same test person.
					// But one handle exists in the outer scope while one exists in the inner scope.
					ManagedPersonHandle testHandle2;

					// NO handles have been created yet, so the reference count should be at 0.
					Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));

					testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
					// After the first handle has been added, we should have a single reference.
					Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));

					testHandle2 = ManagedPersonManager::Instance()->NewHandle(testPerson);
					// Now there are two handles to the test Person, so we should have a reference count of 2.
					Assert::AreEqual(2, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
				}
				// TestHandle2 has gone out of scope but testHandle remains valid, so there should be a 
				// reference count of 1.
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			// Both handles have gone out of scope, so we should have 0 references.
			Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_StaySingle)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			{
				// Calling NewHandle (which called MCManagedHandleManager.CreateRef) and storing the result in
				// the SAME handle should keep the reference count at 1.
				ManagedPersonHandle testHandle;
				Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
				testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}
			ManagedPersonHandle testHandle;
			Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

		TEST_METHOD(ManagedHandleManager_MultipleRefCount_stagered_threaded)
		{
			auto testPerson = std::make_shared<Person>(1, 1);
			auto pTestPerson = testPerson.get();
			std::vector<std::thread> threads;
			{
				ManagedPersonHandle testHandle;
				for (int x = 0; x < 1000; x++) {
					threads.push_back(std::thread([&]()
					{
						{
							ManagedPersonHandle testHandle2;
							{
								// This test will create 2 handles for the same test person.
								// But one handle exists in the outer scope while one exists in the inner scope.
								ManagedPersonHandle testHandle3;
								testHandle = ManagedPersonManager::Instance()->NewHandle(testPerson);
								testHandle2 = ManagedPersonManager::Instance()->NewHandle(testPerson);
								testHandle3 = ManagedPersonManager::Instance()->NewHandle(testPerson);
							}
						}
					}
					));
				}
				for (int x = 0; x < 1000; x++)
					threads[x].join();	
				Assert::AreEqual(1, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
			}

			Assert::AreEqual(0, ManagedPersonManager::Instance()->GetRefCount(pTestPerson));
		}

	};
}