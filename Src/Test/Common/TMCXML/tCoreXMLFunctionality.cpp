#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TMCXML
{		
	TEST_CLASS(TestLoadXMLDocument)
	{
	public:
		
		/*
			Calling MC::Load_MCXML_Document with an invalid filename (a file that does not exist) returns a null unique_ptr.

			No exceptions or other errors should occur.
		*/
		TEST_METHOD(FailOnDocumentMissing)
		{
			auto document = MC::Load_MCXML_Document("c:\\InvalidFileLocation\\InvalidFilename.config");

			Assert::AreEqual(false, (bool)document);
		}

	};
}