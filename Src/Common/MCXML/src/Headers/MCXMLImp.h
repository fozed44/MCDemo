#pragma once

#include "MCXML.h"
#include "pugixml.h"

#define __ENABLE_XML_TRACE

namespace MCXMLImp {

	class MCXML_DocumentImp : public MC::MCXML_Document {
	public:
		virtual ~MCXML_DocumentImp();
	public:
		virtual std::unique_ptr<MC::MCXML_NodeSet> Get(const char * xPath) const override;
	public:
		MCXML_DocumentImp(const MCXML_DocumentImp&)  = delete;
		MCXML_DocumentImp(const MCXML_DocumentImp&&) = delete;
		MCXML_DocumentImp(std::unique_ptr<pugi::xml_document> pDoc);
	private:
		std::unique_ptr<pugi::xml_document> _pDoc;
	};

	class MCXML_NodeSetImp : public MC::MCXML_NodeSet {
	public:
		virtual ~MCXML_NodeSetImp();
	public:
		virtual size_t size() const override;
		virtual std::unique_ptr<MC::MCXML_Node> ElementAt(int32_t pos) override;
	public:
		MCXML_NodeSetImp(const MCXML_NodeSetImp&)  = delete;
		MCXML_NodeSetImp(const MCXML_NodeSetImp&&) = delete;
		MCXML_NodeSetImp(const pugi::xpath_node_set& node);
	private:
		const pugi::xpath_node_set _nodes;
	};

	class MCXML_NodeImp : public MC::MCXML_Node {
	public:
		virtual ~MCXML_NodeImp();
	public:
		virtual void Attribute(const char* const name, int*   result) const override;
		virtual void Attribute(const char* const name, float* result) const override;
		virtual void Attribute(const char* const name, const char **result) const override;
	public:
		MCXML_NodeImp(const MCXML_NodeImp&)  = delete;
		MCXML_NodeImp(const MCXML_NodeImp&&) = delete;
		MCXML_NodeImp(const pugi::xpath_node& node);
	private:
		const pugi::xpath_node _node;
	};


}