#include "jcpu.h"
#include "jnetwork.h"
#include "jmemory.h"

#include <iostream>

using namespace std;

void testCPU()
{
	jphysic::CPU cpu;

	std::cout << cpu.what() << endl;
}

void testNetwork()
{
	jphysic::Network net;

	cout << net.what() << endl;
}

void testMemory()
{
	jphysic::Memory mem;

	cout << mem.what() << endl;
}

int main(int argc, char *argv[])
{
	std::cout << "Testing CPU\n" << endl;
	testCPU();
	std::cout << "Testing Network\n" << endl;
	testNetwork();
	std::cout << "Testing Memory\n" << endl;
	testMemory();
	
	return 0;
}
