#include "../Headers/MCXMLImp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	MCXML_NodeSetImp::MCXML_NodeSetImp(const pugi::xpath_node_set& nodes) 
		: _nodes(nodes) {}

	MCXML_NodeSetImp::~MCXML_NodeSetImp() {}

	size_t MCXML_NodeSetImp::size() const {
		return _nodes.size();
	}

	std::unique_ptr<MC::MCXML_Node> MCXML_NodeSetImp::ElementAt(int32_t pos) {
		pugi::xpath_node xPathNode;

		try {
			if (pos >=(int32_t) _nodes.size()) {
				XML_WARN("Failed to get element {0:d}, reason: pos out of range.", pos);
				return nullptr;
			}
			xPathNode = _nodes[pos];
		}
		catch (pugi::xpath_exception *ex) {
			XML_WARN("Failed to get element {0:d}, reason: {1}", pos, ex->what());
			return nullptr;
		}
		catch (...) {
			XML_WARN("Failed to get element {0:d} for unknown reason.", pos);
			return nullptr;
		}

		return std::unique_ptr<MC::MCXML_Node>(new MCXML_NodeImp(xPathNode));
	}
}