#include "jbufferedreader.h"
#include "jfileinputstream.h"
#include "jfile.h"
#include "jpolices.h"
#include "jioexception.h"
#include "jstringutils.h"
#include "jruntimeexception.h"

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
	Polices p;

	p.Load("file.access");

	return EXIT_SUCCESS;
}

