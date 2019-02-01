#include "../Headers/MCXMLImp.h"

namespace MCXMLImp {

	MCXML_NodeSetImp::MCXML_NodeSetImp(const pugi::xpath_node_set& nodes) 
		: _nodes(nodes) {}

	MCXML_NodeSetImp::~MCXML_NodeSetImp() {}

	size_t MCXML_NodeSetImp::size() const {
		return _nodes.size();
	}

	std::unique_ptr<MC::MCXML_Node> MCXML_NodeSetImp::ElementAt(int32_t pos) {
		pugi::xpath_node xPathNode = _nodes[pos];
		return std::unique_ptr<MC::MCXML_Node>(new MCXML_NodeImp(xPathNode));
	}
}