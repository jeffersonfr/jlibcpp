#include "jcomplex.h"

#include <iostream>

#include <stdlib.h>

int main( int argc, char **argv )
{
	jmath::Complex c(1, 2), d(3, 4), e(0, 0);

	// e = c + d;

	std::cout << "C1:: " << c.what() << ", C2:: " << d.what() << ", SUM:: " << e.what() << std::endl;

	return EXIT_SUCCESS;
}
