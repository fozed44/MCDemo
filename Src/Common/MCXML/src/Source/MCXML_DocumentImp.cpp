#include "../Headers/MCXMLimp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	MCXML_DocumentImp::MCXML_DocumentImp(std::unique_ptr<pugi::xml_document> pDoc)
		: _pDoc(std::move(pDoc)) {}

	MCXML_DocumentImp::~MCXML_DocumentImp() {}

	std::unique_ptr<MC::MCXML_NodeSet> MCXML_DocumentImp::Get(const char* xPath) const {
		pugi::xpath_node_set resultNodeSet;

		XML_TRACE("Searching nodes: {0}", xPath);

		try {
			resultNodeSet = _pDoc->select_nodes(xPath);

			if (resultNodeSet.size() == 0) {
				XML_TRACE("No nodes were found for {0}", xPath);
				return nullptr;
			}
		}
		catch (pugi::xpath_exception *pEx) {
			XML_WARN("Could not select nodes for {0}, reason: {1}", xPath, pEx->what());
			return nullptr;
		}
		catch (...) {
			XML_WARN("Could not select nodes for {0}, reason: unknown", xPath);
			return nullptr;
		}

		XML_TRACE("Pulled {0:d} nodes.", resultNodeSet.size());


		return std::make_unique<MCXML_NodeSetImp>(resultNodeSet);
	}

}