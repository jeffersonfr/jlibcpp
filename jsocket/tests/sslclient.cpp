#include "jsslsocket.h"

#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace jsocket;

int main(void)
{
	SSLSocket mySocket("localhost", 5555);
	
	// Check server certificates agains our known trusted certificate
	mySocket.UseVerification("cert.pem", NULL);
	
	// Print server information
	SSLSocket::peer_cert_info_t info;
	
	mySocket.GetPeerCertInfo(&info);

	printf("Server certificate information:\n");
	printf("Signature algorithm: %s\n", info.sgnAlgorithm.c_str());
	printf("Key algorithm: %s (%d bit)\n", info.keyAlgorithm.c_str(), info.keySize);
	printf("CN   : %s\n", info.commonName.c_str()); 
	printf("C    : %s\n", info.countryName.c_str());
	printf("L    : %s\n", info.localityName.c_str());
	printf("ST   : %s\n", info.stateOrProvinceName.c_str());
	printf("O    : %s\n", info.organizationName.c_str());
	printf("OU   : %s\n", info.organizationalUnitName.c_str());
	printf("T    : %s\n", info.title.c_str());
	printf("I    : %s\n", info.initials.c_str());
	printf("G    : %s\n", info.givenName.c_str());
	printf("S    : %s\n", info.surname.c_str());
	printf("D    : %s\n", info.description.c_str());
	printf("UID  : %s\n", info.uniqueIdentifier.c_str());
	printf("Email: %s\n", info.emailAddress.c_str());
	printf("Valid from %s to %s\n", info.notBefore.c_str(), info.notAfter.c_str());
	printf("Serialnumber: %ld, version: %ld\n", info.serialNumber, info.version);
	
	string pem;
	int r;
	char receive[256];

	mySocket.GetPeerCertPEM(&pem);
	
	printf("\n%s\n\n", pem.c_str());
	
	// Send message to server
	printf("Client running on %s:%d (%s)\n", mySocket.GetInetAddress()->GetHostName().c_str(), mySocket.GetPort(), mySocket.GetInetAddress()->GetHostAddress().c_str());		
	// printf("Sent %d bytes to %s:%d (%s)\n", mySocket.Send("Hello Server!", 13), mySocket.get_peerName().c_str(), mySocket.get_peerPort(), mySocket.get_peerAddr().c_str());
	printf("Sent %d bytes\n", mySocket.Send("Hello Server!", 13));
	
	// Receive message from server
	r = mySocket.Receive(receive, 255);

	receive[r] = 0;

	printf("Received: %s\n", receive);
	
	mySocket.Close();
	
	return 0;
}
