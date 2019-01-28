#pragma once

#include <string>
#include <vector>

namespace MC {

	class MCXML_Node {
	public:
		virtual void Attribute(const char* const name, const int   *result) const = 0;
		virtual void Attribute(const char* const name, const float *result) const = 0;
		virtual void Attribute(const char* const name, const char **result) const = 0;
	};

	class MCXML_NodeSet {
	private:
		std::vector<MCXML_Node*> *_pNodes;
		
	public:
		inline MCXML_Node* begin() { return _pNodes->begin(); }
		inline MCXML_Node* end()   { return _pNodes->end(); }
	};

	class MCXML_Document {
	public:
		virtual MCXML_NODE_SET Get(const char* const xPath) const = 0;
	};
	
	class MCXML {
`	public:
		MCXML_Document Load(const char* const filename);
	};
}
