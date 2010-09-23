#include "jfile.h"
#include "jfileinputstream.h"
#include "jbufferedreader.h"
#include "jstringtokenizer.h"

#include <iostream>

using namespace std;
using namespace jio;

int main()
{
	FileInputStream is("/etc/channels.conf");
	BufferedReader reader(&is);

	while (reader.IsEOF() == false) {
		jcommon::StringTokenizer tokens(reader.ReadLine(), ":", jcommon::SPLIT_FLAG, false);

		for (int i=0; i<tokens.GetSize(); i++) {
			std::cout << tokens.GetToken(i) << std::endl;
		}
	}

	return 0;
}
