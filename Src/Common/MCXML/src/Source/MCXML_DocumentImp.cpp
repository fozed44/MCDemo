#include "../Headers/MCXMLimp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	MCXML_DocumentImp::MCXML_DocumentImp(std::unique_ptr<pugi::xml_document> pDoc)
		: _pDoc(std::move(pDoc)) {}

	MCXML_DocumentImp::~MCXML_DocumentImp() {}

	std::unique_ptr<MC::MCXML_NodeSet> MCXML_DocumentImp::Get(const char* xPath) const {

		XML_TRACE("Searching nodes: {0}", xPath);

		pugi::xpath_node_set resultNodeSet = _pDoc->select_nodes(xPath);

		XML_TRACE("Pulled {0:d} nodes.", resultNodeSet.size());


		return std::make_unique<MCXML_NodeSetImp>(resultNodeSet);
	}

}