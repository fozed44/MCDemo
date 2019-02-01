#pragma once

#include <string>
#include <vector>
#include <memory>

namespace MC {

	class MCXML_Node {
	public:
		virtual void Attribute(const char* const name, int*   result)       const = 0;
		virtual void Attribute(const char* const name, float* result)       const = 0;
		virtual void Attribute(const char* const name, const char **result) const = 0;
		virtual ~MCXML_Node() {};
	public:
		MCXML_Node(const MCXML_Node&) = delete;
		MCXML_Node(const MCXML_Node&&) = delete;
	protected:
		MCXML_Node() {};
	};

	class MCXML_NodeSet {
	public:
		virtual size_t size() const = 0;
		virtual std::unique_ptr<MCXML_Node> ElementAt(int32_t pos) = 0;
		virtual ~MCXML_NodeSet() {};
	public:
		MCXML_NodeSet(const MCXML_NodeSet&)  = delete;
		MCXML_NodeSet(const MCXML_NodeSet&&) = delete;
	protected:
		MCXML_NodeSet() {};
	};

	class MCXML_Document {
	public:
		virtual std::unique_ptr<MCXML_NodeSet> Get(const char* xPath) const = 0;
		virtual ~MCXML_Document() {};
	public:
		MCXML_Document(const MCXML_Document&)  = delete;
		MCXML_Document(const MCXML_Document&&) = delete;
	protected:
		MCXML_Document() {};
	};
	
	std::unique_ptr<MCXML_Document> Load_MCXML_Document(const char* filename);
}
