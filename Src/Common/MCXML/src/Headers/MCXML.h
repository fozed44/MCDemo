#pragma once

#include <string>
#include <vector>
#include "pugixml.h"

namespace MC {

	struct MCXML_NODE {
	private:
		pugi::xpath_node *_pNode;
	public:
		void Attribute(const char* const name, int   *result) const;
		void Attribute(const char* const name, float *result) const;
		void Attribute(const char* const name, const char **result)  const;
	};

	struct MCXML_NODE_SET {
		std::vector<MCXML_NODE> *nodes;
	};

	class MCXML {
	public:
		MCXML(const char* const filename);
		~MCXML();

		MCXML_NODE_SET Get(const char* const xPath);

	private:
		std::string m_filename;
		pugi::doc
	};

}
