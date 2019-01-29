#pragma once


#include "MCXML.h"
#include "pugixml.h"


namespace MCXMLImp {

	class MCXML_DocumentImp : public MC::MCXML_Document {
	public:
		virtual MC::MCXML_NodeSet Get(const char * const xPath) const override;

	public:
		MCXML_DocumentImp(const MCXML_DocumentImp&) = delete;
	private:
		MCXML_DocumentImp(std::unique_ptr<pugi::xml_document> pDoc);
		friend std::unique_ptr<MCXML_Document> MC::MCXML::Load(const char * const filename);
		std::unique_ptr<pugi::xml_document> _pDoc;
	};

	class MCXML_NodeImp : public MC::MCXML_Node {
	public:
		virtual void Attribute(const char* const name, int*   const result) const override;
		virtual void Attribute(const char* const name, float* const result) const override;
		virtual void Attribute(const char* const name, const char **result) const override;
	public:
		MCXML_NodeImp(const MCXML_NodeImp&) = delete;
	private:
		MCXML_NodeImp(std::unique_ptr<pugi::xpath_node> pNode);
		friend MC::MCXML_NodeSet MCXML_DocumentImp::Get(const char * const xPath) const;
		std::unique_ptr<pugi::xpath_node> _pNode;
	};

}