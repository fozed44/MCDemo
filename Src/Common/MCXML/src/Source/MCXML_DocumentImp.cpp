#include "../Headers/MCXMLimp.h"

namespace MCXMLImp {

	MCXML_DocumentImp::MCXML_DocumentImp(std::unique_ptr<pugi::xml_document> pDoc)
		: _pDoc(std::move(pDoc)) {}

	MC::MCXML_NodeSet MCXML_DocumentImp::Get(const char* const xPath) const {
		pugi::xpath_node_set result = _pDoc->select_nodes(xPath);
	}

}