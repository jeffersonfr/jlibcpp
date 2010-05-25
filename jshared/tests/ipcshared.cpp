#include "jsharedlib.h"

#include <iostream>

#include <stdlib.h>

using namespace jshared;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: ./ipcmap <id>" << std::endl;

		return -1;
	}

	SharedMemory *m = NULL;

	int key = atoi(argv[1]);

	try {
		m = new SharedMemory(key, 10000, 0644);
	} catch (...) {
		try {
			m = new SharedMemory(key, 10000);
		} catch (...) {
			std::cout << "Cannot create the shared memory" << std::endl;

			return -1;
		}
	}

	sleep(5);

	return 0;
}

