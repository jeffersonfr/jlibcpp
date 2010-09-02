#include "jpolicies.h"

#include <iostream>

int main() 
{
	jcommon::Policies p;

	p.Load("file.access");

	std::vector<std::string> *v = p.GetPolicies();

	for (std::vector<std::string>::iterator i=v->begin(); i!=v->end(); i++) {
		std::cout << ": " << *i << std::endl;
	}

	return EXIT_SUCCESS;
}

