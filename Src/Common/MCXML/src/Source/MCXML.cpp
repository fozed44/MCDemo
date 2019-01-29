#include "../Headers/MCXMLimp.h"

namespace MC {

	std::unique_ptr<MCXML_Document> MCXML::Load(const char* const filename) {
		std::unique_ptr<pugi::xml_document> document = std::make_unique<pugi::xml_document>();

		pugi::xml_parse_result parseResult = document->load_file(filename);

		if (!parseResult)
			throw std::exception();
			
		return std::make_unique<MCXML_Document>(std::move(document));
	}

}