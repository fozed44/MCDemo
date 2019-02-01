#include "../../../Common/MCLog/src/Headers/MCLog.h"
#include "../../../Common/MCXML/src/Headers/MCXML.h"

#include <iostream>

int main(int argc, char ** argv) {

	MC::MCLog::GetInstance().GetCore()->error("test int: {0:d}", 42);

	MC_ERROR("test float: {0:03.2f}", 42.23);
	//MC_WARN("warning test {0}", 1);

	auto test = MC::Load_MCXML_Document("C:\\Projects\\MCDemo\\bin\\Config\\core.config");

	auto r = test->Get("/note/to");

	int x;

	auto elem = r->ElementAt(0);

	elem->Attribute("testInt", &x);

	char t;
	std::cin >> t;

	return 0;
}