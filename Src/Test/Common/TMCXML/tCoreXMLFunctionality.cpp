#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"
#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include <locale>
#include <codecvt>

#include "windows.h"
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_DIRECTORY "C:\\Temp\\"
#define TEST_FILENAME "validUnitTest.config"
#include <crtdbg.h>

namespace TMCXML
{		


	struct CrtCheckMemory
	{
		_CrtMemState state1;
		_CrtMemState state2;
		_CrtMemState state3;
		CrtCheckMemory()
		{
			_CrtMemCheckpoint(&state1);
		}
		~CrtCheckMemory()
		{
			_CrtMemCheckpoint(&state2);			
			Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2));
			// else just do this to dump the leaked blocks to stdout.
			if (_CrtMemDifference(&state3, &state1, &state2))
				_CrtMemDumpStatistics(&state3);
		}
	};

	std::string  g_validTestFilename;

	void WriteValidTestFile(std::string& filename) {
		std::ofstream testFile;
		testFile.open(filename, std::ios::trunc);
		testFile << "<config>" << std::endl;
		testFile << "<outerElement1>" << std::endl;
		testFile << "<innerElement" << std::endl;
		testFile << "testInt=\"123\"" << std::endl;
		testFile << "testFloat=\"12.12\"" << std::endl;
		testFile << "testString=\"stringValue\"/>" << std::endl;
		testFile << "</outerElement1>" << std::endl;
		testFile << "<outerElement2>" << std::endl;
		testFile << "</outerElement2>" << std::endl;
		testFile << "</config>" << std::endl;
		testFile.close();
	}

	TEST_CLASS(TestLoadXMLDocument)
	{
	public:

		TEST_CLASS_INITIALIZE(TestLoadXMLDocumentInitialize) {
			MC::MCLogHelper::SetNullLoggers();
			CreateDirectoryA(TEST_DIRECTORY, NULL);

			g_validTestFilename = std::string(TEST_DIRECTORY) + std::string(TEST_FILENAME);

			WriteValidTestFile(g_validTestFilename);
		}

		/*
			Calling MC::Load_MCXML_Document with an invalid filename (a file that does not exist) returns a null unique_ptr.

			No exceptions or other errors should occur.
		*/
		TEST_METHOD(FailOnDocumentMissing) {
			auto document = MC::Load_MCXML_Document("c:\\InvalidFileLocation\\InvalidFilename.config");

			Assert::AreEqual(false, (bool)document);
		}

		/*
			Make sure that MC::Load_MCXML_Document() can open a file that we know exists and is valid without throwing any exceptions.
		*/
		TEST_METHOD(OpenValidTestFile) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);

			Assert::AreEqual(true, (bool)document);
		}

		/*
			Read an integer value from an int typed attribute.
		*/
		TEST_METHOD(ReadIntParameterFromValidFile) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);
			int testInt;
			auto result = node->Attribute("testInt", &testInt);

			Assert::AreEqual(123, testInt);
			Assert::IsTrue(result);
		}

		/*
			Read a float value from a float typed attribute.
		*/
		TEST_METHOD(ReadFloatParameterFromValidFile) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);
			float testFloat;
			auto result = node->Attribute("testFloat", &testFloat);

			Assert::AreEqual(12.12f, testFloat);
			Assert::IsTrue(result);
		}

		/*
			Read a string value from a string typed attribute.
		*/
		TEST_METHOD(ReadCharParameterFromValidFile) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);
			std::string testString;
			auto result = node->Attribute("testString", &testString);

			Assert::AreEqual("stringValue", testString.c_str());
			Assert::IsTrue(result);
		}

		/*
			Attempting to do a 'Get' on an MCXML_Document for an xPath that does not exist returns a null node set.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(GetForMissingXPathCreatesNoException) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/nonExistingElement");

			Assert::AreEqual(false, (bool)nodeSet);
		}

		/*
			Attempting to do an 'ElementAt' on an element that is out of range results in an null MCXML_Node.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(ElementAtOutOfBoundsCreatesNoException) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");

			auto node = nodeSet->ElementAt(20);

			Assert::AreEqual(false, (bool)node);
		}

		/*
			Attempting to make a call to MCXML_Node.Attribute() for an int value that doesn't exist results in a return value of false.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(AttributeIntNotExistsCreatesNoException) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);

			int testInt;
			auto result = node->Attribute("doesntExist", &testInt);

			Assert::IsFalse(result);
		}

		/*
			Attempting to make a call to MCXML_Node.Attribute() for a float value that doesn't exist results in a return value of false.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(AttributeFloatNotExistsCreatesNoException) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);

			float testFloat;
			auto result = node->Attribute("doesntExist", &testFloat);

			Assert::IsFalse(result);
		}

		/*
			Attempting to make a call to MCXML_Node.Attribute() for a char value that doesn't exist results in a return value of false.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(AttributeStringNotExistsCreatesNoException) {
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);

			std::string testString;
			auto result = node->Attribute("doesntExist", &testString);

			Assert::IsFalse(result);
		}

		/*
			Attempting to make a call to MCXML_Node.Attribute() for a int that cant be parsed as an int returns false.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(AttributeIntCantParseCreatesNoException) {
			CrtCheckMemory();

			int * t = new int[50000];
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);

			int testInt;
			auto result = node->Attribute("testFloat", &testInt);

			Assert::IsFalse(result);
		}

		/*
			Attempting to make a call to MCXML_Node.Attribute() for a float value that cant be parsed as a float returns false.

			Make sure that no exceptions are thrown.
		*/
		TEST_METHOD(AttributeFloatCantParseCreatesNoException) {
			CrtCheckMemory();
			auto document = MC::Load_MCXML_Document(g_validTestFilename);
			auto nodeSet = document->Get("config/outerElement1/innerElement");
			auto node = nodeSet->ElementAt(0);

			float testFloat;
			auto result = node->Attribute("testString", &testFloat);

			Assert::IsFalse(result);
		}

	};
}