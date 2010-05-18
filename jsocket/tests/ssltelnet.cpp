/**
 * SSL telnet
 *
 * A bit more advanced example than the simple server & client examples.
 * Makes a "telnet" connection to an SSL server. Uses fork() to handle two
 * dataflows; select() should really be used instead if you need to handle 
 * more complex situations.
 *
 */

#include "jsslsocket.h"
#include "jsslserversocket.h"

#include <string>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;
using namespace jsocket;

bool interrupted = false;

void sighandler(int signum)
{
	interrupted = true;

	signal(SIGINT, SIG_DFL); // Activate ctrl+c again if we screwup
}

int main(void)
{
	string host = "localhost";
	int port    = 5555;

	std::cout << "Connecting to " << host << ":" << port << std::endl;

	// WARN:: try-catch
	SSLSocket mySocket(host, port);
	
	//Try to connect to socket
	int i = 0;
	while(true){
		//No response, try again later
		if( i > 20 ){
			std::cout << "\nClient timeout !" << std::endl;
			exit(-1);
		}
		
		std::cout << "." << std::flush;
		sleep(3);
		i++;
	}
	
	// Create a new process for handling input from stdin and then handle stdout ourself
	pid_t pid;
	if( (pid = fork()) == 0 ){
		//send process
	
		char buf[256];
		int size;
		buf[255] = '\0';
		
		while( true ){
			fgets(buf, 253, stdin); // read one line from stdin
			
			size = strlen(buf);
			if(size > 253)
				size=253;
			
			// Add \r\n (CRLF - standard telnet end of line)
			buf[size] = '\r';
			buf[size+1] = '\n';
			
			mySocket.Send(buf, size+2); // send to server
		}
	
		return 0;
	}
	
	// recive process
		
	int size = 0;
	char buf[257];
	buf[256] = '\0';
	
	// Handle ctrl+c
	signal(SIGINT, sighandler);  // our sighandler() function will be called on ctrl+c
		
	while( true ){
		size = mySocket.Receive(buf, 256);
		
		// handle errors and interruptions
		// INFO:: repetir esse codigo em caso de excecao
		if(interrupted == true ){
			kill(pid, SIGKILL); // Kill send process
			wait(NULL);         // Wait for send process to die
				
			if (interrupted == true ){
				std::cout << "\nConnection to " << host << " was closed" << std::endl;

				mySocket.Close();
				
				return 0;
			}
			
			// something went wrong
			mySocket.Close();

			return -1;
		}
				
		write(1, buf, size);  // write to stdout
	}
	
	return 0;
}
