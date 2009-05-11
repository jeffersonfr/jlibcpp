#include "jdatagramsocket.h"
#include "jsocketlib.h"
#include "jfileinputstream.h"
#include "jbufferedreader.h"
#include "jstringtokenizer.h"
#include "jsocket.h"

#include <iostream>
#include <stdexcept>

#include <fcntl.h>

using namespace jsocket;

#define BUFFER_SIZE	4096

jio::FileInputStream *file = NULL;
jio::BufferedReader *reader = NULL;
std::vector<Connection *> hosts;
int receive_port = 0;

int init_args(int argc, char **argv)
{
	if (argc < 3) {
		return -1;
	}
	
	try {
		file = new jio::FileInputStream(argv[2]);
		reader = new jio::BufferedReader(file);

		receive_port = atoi(argv[1]);
	} catch (...) {
		if (file != NULL) {
			file->Close();
			delete file;
			file = NULL;
		}
		
		if (reader != NULL) {
			delete reader;
			reader = NULL;
		}

		return -1;
	}

	Connection *host;
	std::string line;

	while (reader->EndOfFile() == false) {
		try {
			line = reader->ReadLine();
			
			jcommon::StringTokenizer token(line, ":", jcommon::SPLIT_FLAG, false);
			
			if (token.GetSize() == 2) {
				host = new DatagramSocket(token.GetToken(0), atoi(token.GetToken(1).c_str())); // host, port

				hosts.push_back(host);
			}
		} catch (...) {
		}
	}

	file->Close();
	
	return 0;
}

int init_proxy()
{
	// TODO:: testar no win32
	
	DatagramSocket server(receive_port);
	DatagramSocket *udp;
	
	char buffer[BUFFER_SIZE];
	int length;
	
	std::vector<Connection *>::iterator i;

	while (true) {
		try {
			length = server.GetInputStream()->Read(buffer, BUFFER_SIZE);
			
			std::cout << "Sending:: " << length << " B/s\r" << std::flush;

			if (length > 0) {
				for (i=hosts.begin(); i!=hosts.end(); i++) {
					udp = (DatagramSocket *)(*i);
					
					if (udp->GetOutputStream()->Write(buffer, length) < 0) {
						std::cout << "Cliente " << udp->GetInetAddress()->GetHostName() << " morto !" << std::endl;
					}
				}
			}
		} catch (std::runtime_error &e) {
			std::cout << "error in init proxy" << e.what() << std::endl;

			return -1;
		}
	}

	return 0;
}

void help(char *p)
{
	std::cout << "JProxy\n" << std::endl;
	std::cout << "use: " << p << " <recv_port> <file_hosts>\n" << std::endl;

	exit(1);
}

int main(int argc, char **argv)
{
	InitWindowsSocket();

	if (init_args(argc, argv) == -1) {
		help(argv[0]);
	}

	if (init_proxy() == -1) {
		return -1;
	}

	ReleaseWindowsSocket();

	return 0;
}

