#pragma once

#include <string>
#include <vector>
#include <memory>

namespace MC {

	class MCXML_Node {
	public:
		virtual void Attribute(const char* const name, int* const result) const = 0;
		virtual void Attribute(const char* const name, float* const result) const = 0;
		virtual void Attribute(const char* const name, const char **result) const = 0;
	};

	class MCXML_NodeSet {
	private:
		std::vector<MCXML_Node*> *_pNodes;
		
	public:
		inline std::vector<MCXML_Node*>::iterator begin() { return _pNodes->begin(); }
		inline std::vector<MCXML_Node*>::iterator end()   { return _pNodes->end(); }
	};

	class MCXML_Document {
	public:
		virtual MCXML_NodeSet Get(const char* const xPath) const = 0;
		virtual ~MCXML_Document() = 0;
	};
	
	class MCXML {
	public:
		static std::unique_ptr<MCXML_Document> Load(const char* const filename);
	};
}
