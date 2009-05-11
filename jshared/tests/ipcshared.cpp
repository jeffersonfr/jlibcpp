#include "jsharedmemory.h"

using namespace jshared;

int main(int argc, char **argv)
{
	if (argc < 2) {
		puts("use:: ./ipcmap <id>");

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
			puts("Cannot create the shared memory");

			return -1;
		}
	}

	sleep(5);

	return 0;
}

