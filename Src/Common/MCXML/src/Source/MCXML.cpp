#include "../Headers/MCXMLimp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MC {

	std::unique_ptr<MCXML_Document> Load_MCXML_Document(const char* filename) {
		auto document = std::make_unique<pugi::xml_document>();

		XML_TRACE("Opening xml file: {0}", filename);

		auto parseResult = document->load_file(filename);

		if (!parseResult)
			return nullptr;
			
		return std::make_unique<MCXMLImp::MCXML_DocumentImp>(std::move(document));
	}

}