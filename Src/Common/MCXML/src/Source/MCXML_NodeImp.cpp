#include "../Headers/MCXMLImp.h"
#include "../../../MCLog/src/Headers/MCLog.h"

namespace MCXMLImp {

	static bool isFloatNumber(const std::string& string) {
		std::string::const_iterator it = string.begin();
		bool decimalPoint = false;
		int minSize = 0;
		if (string.size()>0 && (string[0] == '-' || string[0] == '+')) {
			it++;
			minSize++;
		}
		while (it != string.end()) {
			if (*it == '.') {
				if (!decimalPoint) decimalPoint = true;
				else break;
			}
			else if (!std::isdigit(*it) && ((*it != 'f') || it + 1 != string.end() || !decimalPoint)) {
				break;
			}
			++it;
		}
		return (int)string.size()>minSize && it == string.end();
	}

	inline bool isInteger(const std::string & s)
	{
		if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

		char * p;
		strtol(s.c_str(), &p, 10);

		return (*p == 0);
	}

	MCXML_NodeImp::MCXML_NodeImp(const pugi::xpath_node& node)
		: _node(node) {}

	MCXML_NodeImp::~MCXML_NodeImp() {}

	bool MCXML_NodeImp::Attribute(const char* const name, int* result) const {

		XML_TRACE("Reading attribute: {0} as int", name);

		try {
			auto stringResult = _node.node().attribute(name).as_string(nullptr);

			if (stringResult == nullptr) {
				MC_ERROR("The xml attribute {0} does not exist.", name);
				XML_ERROR("The xml attribute {0} does not exist.", name);
				return false;
			}

			if (!isInteger(stringResult)) {
				MC_ERROR("Could not convert xml attribute {0} (value: {1}) to an int, the attribute does not exist or is empty.", name, stringResult);
				XML_ERROR("Could not convert xml attribute {0} (value: {1}) to an int, the attribute does not exist or is empty.", name, stringResult);
				return false;
			}

			*result = _node.node().attribute(name).as_int();
		}
		catch (pugi::xpath_exception *ex) {
			MC_ERROR("Did not find XML attribute: {0}, error: ", name, ex->what());
			XML_ERROR("Did not fine attribute: {0}, error: ", name, ex->what());
			return false;
		}
		catch (...) {
			MC_ERROR("Did not find XML attribute: {0}, error: unknown.", name);
			XML_ERROR("Did not fine attribute: {0}, error: unknown.", name);
			return false;
		}

		XML_TRACE("Value: {0:d}", *result);

		return true;
	}

	bool MCXML_NodeImp::Attribute(const char* const name, float* result) const {

		XML_TRACE("Reading attribute: {0} as float", name);
		
		try {
			auto stringResult = _node.node().attribute(name).as_string(nullptr);

			if (stringResult == nullptr) {
				MC_ERROR("The xml attribute {0} does not exist.", name);
				XML_ERROR("The xml attribute {0} does not exist.", name);
				return false;
			}

			if (!isFloatNumber(stringResult)) {
				MC_ERROR("Could not convert xml attribute {0} (value: {1}) to float, the attribute does not exist or is empty.", name, stringResult);
				XML_ERROR("Could not convert xml attribute {0} (value: {1}) to float, the attribute does not exist or is empty.", name, stringResult);
				return false;
			}
			*result = _node.node().attribute(name).as_float();
		}
		catch (pugi::xpath_exception *ex) {
			MC_ERROR("Did not find XML attribute: {0}, error: ", name, ex->what());
			XML_ERROR("Did not fine attribute: {0}, error: ", name, ex->what());
			return false;
		}
		catch (...) {
			MC_ERROR("Did not find XML attribute: {0}, error: unknown.", name);
			XML_ERROR("Did not fine attribute: {0}, error: unknown.", name);
			return false;
		}

		XML_TRACE("Value: {0:f}", *result);

		return true;
	}

	bool MCXML_NodeImp::Attribute(const char* const name, std::string *pResult) const {

		XML_TRACE("Reading attribute: {0} as char*", name);

		try {
			auto stringValue = _node.node().attribute(name).as_string(nullptr);
			if (stringValue == nullptr) {
				MC_ERROR("The xml attribute {0} does not exist.", name);
				XML_ERROR("The xml attribute {0} does not exist.", name);
				return false;
			}
			*pResult = _node.node().attribute(name).value();
		}
		catch (pugi::xpath_exception *ex) {
			MC_ERROR("Did not find XML attribute: {0}, error: ", name, ex->what());
			XML_ERROR("Did not fine attribute: {0}, error: ", name, ex->what());
			return false;
		}
		catch (...) {
			MC_ERROR("Did not find XML attribute: {0}, error: unknown.", name);
			XML_ERROR("Did not fine attribute: {0}, error: unknown.", name);
			return false;
		}

		XML_TRACE("Value: {0}", pResult->c_str());

		return true;
	}
}