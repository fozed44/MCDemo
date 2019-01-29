#include "../Headers/MCXMLImp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	MCXML_NodeImp::MCXML_NodeImp(std::unique_ptr<pugi::xpath_node> pNode)
		: _pNode(std::move(pNode)) {}

	void MCXML_NodeImp::Attribute(const char* const name, int* const result) const {

		MC_TRACE(std::string("reading attribute: ") + name + " as int.");

		*result = _pNode->node().attribute(name).as_int();

		MC_TRACE(std::string("Value: ") + std::string(*result));
	}

	void MCXML_NodeImp::Attribute(const char* const name, float* const result) const {

		MC_TRACE(std::string("reading attribute: ") + name + " as float.");

		*result = _pNode->node().attribute(name).as_float();

		MC_TRACE(std::string("Value: ") + std::string(*result));
	}

	void MCXML_NodeImp::Attribute(const char* const name, const char **result) const {

		MC_TRACE(std::string("Reading attribute: ") + name + " as char*.");

		*result = _pNode->node().attribute(name).value();

		MC_TRACE(std::string("Value: ") + std::string(*result));
	}
}