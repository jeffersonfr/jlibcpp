#include "jfileinputstream.h"
#include "jfileoutputstream.h"
#include "jdatainputstream.h"
#include "jdataoutputstream.h"

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <list>
#include <map>

using namespace std;
using namespace jio;
using namespace jcommon;

int main() 
{
	FileOutputStream *fos = new FileOutputStream("/tmp/obj");
	FileInputStream *fis = new FileInputStream("/tmp/obj");
	DataOutputStream *oos = new DataOutputStream(fos);
	DataInputStream *ois = new DataInputStream(fis);
	char c;
	short s;
	int i;
	long long l;
	uint8_t uc;
	uint16_t us;
	uint32_t ui;
	uint64_t ul;
	
	oos->Write((uint8_t)1);
	oos->Write((uint16_t)1);
	oos->Write((uint32_t)1);
	oos->Write((uint64_t)1);

	oos->Flush();

	ois->Read((uint8_t *)&c);
	ois->Read((uint16_t *)&s);
	ois->Read((uint32_t *)&i);
	ois->Read((uint64_t *)&l);

	ois->Reset();

	ois->Read(&uc);
	ois->Read(&us);
	ois->Read(&ui);
	ois->Read(&ul);

	printf("char[%d], short[%d], int[%d], long[%lld]\n", c, s, i, l);
	printf("uchar[%u], ushort[%u], uint[%u], ulong[%llu]\n", uc, us, ui, ul);

	delete oos;
	delete ois;
	delete fos;
	delete fis;

	return EXIT_SUCCESS;
}

