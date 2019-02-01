#include "../Headers/MCXMLImp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	MCXML_NodeImp::MCXML_NodeImp(const pugi::xpath_node& node)
		: _node(node) {}

	MCXML_NodeImp::~MCXML_NodeImp() {}

	void MCXML_NodeImp::Attribute(const char* const name, int* result) const {

		XML_TRACE("Reading attribute: {0} as int", name);

		*result = _node.node().attribute(name).as_int();

		XML_TRACE("Value: {0:d}", *result);
	}

	void MCXML_NodeImp::Attribute(const char* const name, float* result) const {

		XML_TRACE("Reading attribute: {0} as float", name);

		*result = _node.node().attribute(name).as_float();

		XML_TRACE("Value: {0:f}", *result);
	}

	void MCXML_NodeImp::Attribute(const char* const name, const char **result) const {

		XML_TRACE("Reading attribute: {0} as char*", name);

		*result = _node.node().attribute(name).value();

		XML_TRACE("Value: {0}", *result);
	}
}