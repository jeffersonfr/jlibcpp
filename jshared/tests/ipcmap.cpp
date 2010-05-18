#include "jmemorymap.h"

#include <stdio.h>

using namespace jshared;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: ./ipcmap <id>" << std::endl;

		return -1;
	}

	MemoryMap *m = NULL;

	std::string key;

	if (argc == 1) {
		key = "server";
	} else if (argc > 1) {
		key = argv[1];
	}

	try {
		m = new MemoryMap(key, MEM_CREAT, jshared::MEM_WRITE, true);
	} catch (...) {
		try {
			m = new MemoryMap(key, MEM_OPEN, jshared::MEM_WRITE, true);
		} catch (...) {
			std::cout << "Cannot create the memory map" << std::endl;

			return -1;
		}
	}

	sleep(100);

	return 0;
}

