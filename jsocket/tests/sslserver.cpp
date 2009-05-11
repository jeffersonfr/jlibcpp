#include "jsslserversocket.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace jsocket;

int main(void)
{
	SSLServerSocket listener(5555);
	SSLSocket *s;
	
	// Load our certificate
	listener.UseCertPassword("cert.pem", "cert_key.pem", "qwerty");
	listener.UseDHFile("dh1024.pem");

	while(true){
		s = (SSLSocket *)listener.Accept();
		
		// Receive message
		char msg[256];
		int r;

		r = s->Receive(msg, 255);

		if (r > 0) {
			msg[r] = 0;

			printf("Received [%d]: %s\n", r, msg);

			s->Send("Hello Client!", 13);
		} else {
			printf("Erro no receive\n");
		}

		s->Close();

		delete s;

		return 0;

		delete s;
		s = NULL;
	}
	
	return 0;
}
