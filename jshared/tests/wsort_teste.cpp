#include "jsharedlib.h"

#include <iostream>

using namespace jshared;

int main() 
{
	// string s = "Peter Winston\nPhilip Johnson\nAnna Anderson\n";
	std::string s = "<html>";

	// WrapperProcess sort("sort -k 2");
	WrapperProcess sort("php");

	sort.Write(s);
	// sort.Close();

	char buf[256 + 1];
	int r;

	while ((r = sort.Read(buf, 256)) > 0) {
		buf[r] = '\0';
		
		std::cout << buf << std::endl;
	}

	return 0;
}
