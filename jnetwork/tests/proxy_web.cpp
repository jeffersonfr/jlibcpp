/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
 * Simple WEB Proxy for Linux and perhaps other systems
 *
 *             This program is used to redirect HTTP requests it receives to the right HTTP server, or to
 *             another instance of itself, on an other host. It acts like a proxy and all the Web browsers
 *             that will have to use it must be setup to use it as the HTTP Proxy. It then allows several
 *             hosts on a network to access the Web via one only server, which is particularly interesting
 *             in case of a server connected to an Internet provider via a modem with PPP.
 *
 *             One interesting aspect is that it doesn't require superuser privileges to run  :-)
 *
 * Authors:    based on stuff by
 *             Willy Tarreau <tarreau@aemiaif.ibp.fr>
 *             Pavel Krauz <kra@fsid.cvut.cz>
 * 
 *             Multithreaded code, POST http method, SIGPIPE, fixes, (rework)
 * 
 * Todo:       - make a list of hosts and network which can be accessed directly, and those which need another proxy
 *             - add an option to supply an access log with hostnames and requests
 */
#include "jnetwork/jserversocket.h"
#include "jnetwork/jsocket.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jsemaphoretimeoutexception.h"

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define METHOD_GET			1
#define METHOD_POST			2
#define METHOD_HEAD			3

#define ERR_SOCKET			-5
#define ERR_GETHOSTBYNAME	-6
#define ERR_CONNECT			-7
#define ERR_CONNECT_TIMEOUT	-8

// default listen port
#define LISTENPORT			8080

// default timeout for any read or write, in seconds 
#define TIMEOUT_OUT			6

// exit timeout for idle thread
#define TIMEOUT_THREAD_EXIT	15

// length of data buffer in chars 
#define LDATA				1024

// length of remote server address 
#define LADR				128

// default port to connect to if unspecified 
#define DEFAULTPORT			80

// max proxy threads for requests
#if 1
#define MAX_PROXY_THREADS	64
#else
#define MAX_PROXY_THREADS	4
#endif

int ConnectToProxy = 0;		// 1 here means this program will connect to another instance of it 0 means we'll connect directly to the Internet 
char NextProxyAdr[128];		// the name of the host where the next instance of the program runs 
int NextProxyPort;			// and its port 
int NoCache = 0;			// if not 0, prevents web browsers from retrieving pages in their own cache when the users does a "Reload" action 
int timeout_out = TIMEOUT_OUT;
int max_proxy_threads = MAX_PROXY_THREADS;

struct th_proxy_struct {
	struct th_proxy_struct *next_free;
  std::mutex mu;
  std::condition_variable cond;
	jnetwork::Socket *sock_in;
};

// global variables
std::mutex free_q_mu;
std::condition_variable free_q_cond;
struct th_proxy_struct *free_q;
int thread_count = 0;		// protected with free_q_mu 

// functions prototype
void help();
struct th_proxy_struct *new_proxy_th(void);
struct th_proxy_struct *alloc_proxy_th(void);
void server(int port);
void sayerror(char *msg, jnetwork::Socket *sockIn, jnetwork::Socket *sockOut);
int process_request(jnetwork::Socket *sockIn);

// global constants
char *BADREQ = (char *)
	"HTTP/1.0 400 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>400 Bad Request</TITLE></HEAD>\n"
	"<BODY><H1>400 Bad Request</H1>\n"
	"Your client sent a query that this server could not\n"
	"understand.<P>\n"
	"Reason: Invalid or unsupported method.<P>\n"
	"</BODY>\n";

char *SERVERR = (char *)
	"HTTP/1.0 500 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>500 Server Error</TITLE></HEAD>\n"
	"<BODY><H1>500 Server Error</H1>\n"
	"Internal proxy error while processing your query.<P>\n"
	"Reason: Internal proxy error.<P>\n"
	"</BODY>\n";

char *SERVCOERR = (char *)
	"HTTP/1.0 500 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>500 Server Error</TITLE></HEAD>\n"
	"<BODY><H1>500 Server Error</H1>\n"
	"Internal proxy error while processing your query.<P>\n"
	"Reason: Invalid connection.<P>\n"
	"</BODY>\n";

char *SERVDNSERR = (char *)
	"HTTP/1.0 500 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>500 Server Error</TITLE></HEAD>\n"
	"<BODY><H1>500 Server Error</H1>\n"
	"Internal proxy error while processing your query.<P>\n"
	"Reason: Bad address - DNS cann't resolve address.<P>\n"
	"</BODY>\n";

char *SERVTIMEOUT = (char *)
	"HTTP/1.0 500 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>500 Server Error</TITLE></HEAD>\n"
	"<BODY><H1>500 Server Error</H1>\n"
	"Internal proxy error while processing your query.<P>\n"
	"Reason: Server time out while connection establishment or data transfer.<P>\n"
	"</BODY>\n";

char *POSTERR = (char *)
	"HTTP/1.0 500 ERROR\r\n"
	"Server: thproxyd\r\n"
	"Content-type: text/html\r\n"
	"\r\n"
	"<HEAD><TITLE>500 Proxy Server Error</TITLE></HEAD>\n"
	"<BODY><H1>500 Proxy Server Error</H1>\n"
	"Failed to POST.<P>\n"
	"Reason: post method error ???.<P>\n"
	"</BODY>\n";

/* displays the right syntax to call Webroute */
void help()
{
	std::cout << "Syntax:" << std::endl;
	std::cout << "webroute [ -p port ] [ -x h:p ] [ -t timeout ] [ -m max_threads ] [ -n ]" << std::endl;
	std::cout << "Available options are:" << std::endl;
	std::cout << "  -p allows you to run webroute on the port <port>." << std::endl;
	std::cout << "     If you don't have superuser privileges, you must use a port > 5000." << std::endl;
	std::cout << "     The default port is " << LISTENPORT << std::endl;
	std::cout << "  -x enables multi-proxy feature. This means that this instance of Webroute" << std::endl;
	std::cout << "     doesn't have itself access to the internet, but the one which is running" << std::endl;
	std::cout << "     on port <p> of host <h> can provide an access. It's possible to chain" << std::endl;
	std::cout << "     as many instances of Webroute as you want. That depends of your network topology" << std::endl;
	std::cout << "  -t <timeout> specifies how many seconds the connection will stay connected (or trying to connect) when" << std::endl;
	std::cout << "     no data arrives. After this time, the connection will be canceled." << std::endl;
	std::cout << "  -n prevents browsers from retrieving web pages from their own cache when the" << std::endl;
	std::cout << "     user asks for a \"Reload\". The page will then always be reloaded." << std::endl;
	std::cout << "  -m max count of proxy threads allocated to serve the requests" << std::endl;
}

class Client {

	private:
		struct th_proxy_struct *th_proxy;
    std::thread _thread;

	public:
		Client(struct th_proxy_struct *th)
		{
			th_proxy = th;
		}

		virtual ~Client()
		{
      _thread.join();
		}

    virtual void Start()
    {
      _thread = std::thread(&Client::Run, this);
    }

		virtual void Run()
		{
			struct th_proxy_struct **th;

			signal(SIGPIPE, SIG_IGN);
			for (;;) {
        std::unique_lock<std::mutex> lock(th_proxy->mu);

				while (th_proxy->sock_in == nullptr) {
          std::chrono::steady_clock::time_point point = std::chrono::steady_clock::now();

          point = point + std::chrono::seconds(TIMEOUT_THREAD_EXIT);
					
					try {
						th_proxy->cond.wait_for(lock, std::chrono::seconds(TIMEOUT_THREAD_EXIT));
					} catch (jexception::SemaphoreTimeoutException &e) {
						free_q_mu.lock();
						th = &free_q;
						while (*th && *th != th_proxy)
							th = &((*th)->next_free);
						if (*th == th_proxy) {
							// remove yourself from queue and exit
							*th = th_proxy->next_free;
							thread_count--;
							free_q_mu.unlock();
							free(th_proxy);
							goto run_exit;
						}
						free_q_mu.unlock();
					}
				}

				th_proxy->mu.unlock();

				process_request(th_proxy->sock_in);

				lock.lock();

				th_proxy->sock_in = nullptr;
				th_proxy->mu.unlock();

				free_q_mu.lock();
				th_proxy->next_free = free_q;
				free_q = th_proxy;
				free_q_mu.unlock();
				free_q_cond.notify_one();
			}

			run_exit:
				return;
		}
};

struct th_proxy_struct *new_proxy_th(void)
{
	struct th_proxy_struct *th_proxy = nullptr;

	try {
		th_proxy = new struct th_proxy_struct;

		memset(th_proxy, 0, sizeof(struct th_proxy_struct));

		th_proxy->next_free = nullptr;
		th_proxy->sock_in = nullptr;

		Client *client = new Client(th_proxy);

		client->Start();
	} catch (std::bad_alloc &e) {
		if ((void *)th_proxy != nullptr) {
			delete th_proxy;
		}

		return nullptr;
	}

	return th_proxy;
}

struct th_proxy_struct *alloc_proxy_th(void)
{
	struct th_proxy_struct *th_proxy;

  std::unique_lock<std::mutex> lock(free_q_mu);

	do {
		th_proxy = free_q;

		if (free_q) {
			free_q = free_q->next_free;
		} else {
			if (thread_count < max_proxy_threads) {
				if ((th_proxy = new_proxy_th())) {
					thread_count++;
				}
			}
			if (!th_proxy) {
				free_q_cond.wait(lock);
			}
		}
	} while (!th_proxy);

	return th_proxy;
}

void server(int port)
{
	struct th_proxy_struct *th_proxy;

	jnetwork::ServerSocket server(port);
	jnetwork::Socket *s;

	for (;;) {
		try {
			s = server.Accept();
		
			th_proxy = alloc_proxy_th();

			th_proxy->mu.lock();
			th_proxy->sock_in = s;
			th_proxy->mu.unlock();

			th_proxy->cond.notify_all();
		} catch (jexception::Exception &e) {
			perror("Broken connection");
		}
	}
}

/* this function should be called only by the child */
void sayerror(char *msg, jnetwork::Socket *sockIn, jnetwork::Socket *sockOut)
{
	sockIn->Send(msg, strlen(msg));
	
	jnetwork::SocketOptions *o = sockIn->GetSocketOptions();

	o->SetLinger(1, 4);

	delete o;

	o = sockOut->GetSocketOptions();

	o->SetLinger(1, 1);

	delete o;

	sockOut->Close();
	sockIn->Close();
}

int process_request(jnetwork::Socket *sockIn)
{
	char data[LDATA];
	char adr[LADR], *p;
	int ldata, lreq, port, req_len, req_method;
	jnetwork::Socket *sockOut = nullptr;
	
	jnetwork::SocketOptions *o = sockIn->GetSocketOptions();

	o->SetReuseAddress(true);
	o->SetKeepAlive(true);

	delete o;

	// here, we'll analyze the request and get rid of "http://adr:port". The address and port willbe duplicated and used to open the connection
	if (sockIn->Receive(data, LDATA) <= 0) {
		goto badreq;
	}

	// it's easy to log all requests here  fprintf(stderr,"requete recue: %s",data); 
	ldata = strlen(data);
	
	if (strncmp(data, "GET ", 4) == 0) {
		req_len = 4;
		req_method = METHOD_GET;
	} else if (strncmp(data, "POST ", 5) == 0) {
		req_len = 5;
		req_method = METHOD_POST;
	} else if (strncmp(data, "HEAD ", 5) == 0) {
		req_len = 5;
		req_method = METHOD_HEAD;
	} else
		goto badreq;

	if (!ConnectToProxy) {	/* if proxy-to-proxy connection, we don't modify the request */
		char *str;

		str = data + req_len;
		while (*str == ' ')
			str++;
		if (!strncmp(str, "http://", 7))
			str += 7;
		if ((p = strchr(str, '/')) != nullptr) {
			strncpy(adr, str, (p - str));	/* copies addresse in adr */
			adr[p - str] = 0;
			str = p;	/* points to the rest of the request (without address) */
			lreq = ldata - (str - data);
		} else
			goto badreq;	/* if no /, error */
		/* at this stage, adr contains addr[:port], and str points to the local URL with the first  '/' */
		if (adr[0] == 0)
			goto badreq;
		p = strchr(adr, ':');
		if (p == nullptr)	/* unspecified port. The default one will be used */
			port = DEFAULTPORT;
		else {		/* port is available. let's read it */
			*(p++) = 0;	/* ends hostname */
			port = atoi(p);
		}
		/* end of request analysis. The hostname is in "adr", and the port in "port" */
		try {
			sockOut = new jnetwork::Socket(adr, port);
		} catch (jexception::ConnectionException &e) {
			switch (errno) {
				case ERR_GETHOSTBYNAME:
					goto servdnserr;
			}
			goto servcoerr;
		}
		/* As it becomes a local URL, we only say "GET" and the end of the request. */
		switch (req_method) {
		case METHOD_GET:
			sockOut->Send("GET ", 4);
			break;
		case METHOD_POST:
			sockOut->Send("POST ", 5);
			break;
		case METHOD_HEAD:
			sockOut->Send("HEAD ", 5);
			break;
		}
		sockOut->Send(str, lreq);
	} else {		/* proxy-to-proxy connection ! */
		try {
			sockOut = new jnetwork::Socket(NextProxyAdr, NextProxyPort);
		} catch (jexception::ConnectionException &e) {
			switch (errno) {
				case ERR_GETHOSTBYNAME:
					goto servdnserr;
			}
			goto servcoerr;
		}
	}
	/* now, let's copy all what we don't have copied yet */
	if (req_method == METHOD_POST) {
		int c_len = 0;
		char *p;

		do {
			if (sockIn->Receive(data, LDATA) <= 0) {
				break;
			}
			ldata = strlen(data);
			if (strncasecmp(data, "Content-Length", 14) == 0) {
				p = data + 14;
				while (*p != ':') {
					p++;
				}
				c_len = atoi(++p);
			}
			sockOut->Send(data, ldata);
		} while (ldata && data[0] != '\n' && data[0] != '\r');
		if (c_len == 0) {
			goto posterr;
		}
		while (c_len) {
			ldata = sockIn->Receive(data, (LDATA > c_len ? c_len : LDATA));
			sockOut->Send(data, ldata);
			c_len -= ldata;
		}
	} else { /*
		  * METHOD_GET, METHOD_HEAD
		  */
		do {
			if (sockIn->Receive(data, LDATA) <= 0) {
				break;
			}
			ldata = strlen(data);
			if (!NoCache || (strncmp(data, "If-Mod", 6))) {
				sockOut->Send(data, ldata);
			}
		} while (ldata && data[0] != '\n' && data[0] != '\r');
	}
	/* retrieve data from server */
	do {
		int err;
		do {
			ldata = sockOut->Receive(data, LDATA);
		} while (ldata == -1 && errno == EINTR);	/* retry on interrupt */
		if (ldata < 0)
			goto serverr;
		if (ldata) {	/* if ldata > 0, it's not the end yet */
			do {
				err = sockIn->Send(data, ldata);
			} while (err == -1 && errno == EINTR);
			if (errno == EPIPE) {	/* other end (client) closed the conection */
				goto end;
			}
			if (err == -1)
				goto serverr;
		}
	} while (ldata > 0);	/* loops while more data available */

	end:
	sockIn->Close();
	sockOut->Close();
	return 0;	
    
	badreq:
	sayerror(BADREQ, sockIn, sockOut);
	return -1;
    
	serverr:
	sayerror(SERVERR, sockIn, sockOut);
	return -2;
    
	servcoerr:
	sayerror(SERVCOERR, sockIn, sockOut);
	return -4;
    
	servdnserr:
	sayerror(SERVDNSERR, sockIn, sockOut);
	return -5;
    
	posterr:
	sayerror(POSTERR, sockIn, sockOut);
	return -6;
}

int main(int argc, char **argv)
{
	int listenport = LISTENPORT;
	int opt;

	while ((opt = getopt(argc, argv, "p:x:t:nm:")) != -1) {
		switch (opt) {
		case ':':	/* missing parameter */
		case '?':	/* unknown option */
			help();
			exit(1);
		case 'p':	/* port */
			listenport = atoi(optarg);
			break;
		case 'x':	/* external proxy */
			char *p;
			p = strchr(optarg, ':');
			if (p == nullptr) {	/* unspecified port number. let's quit */
				std::cout << "missing port for next proxy" << std::endl;
				help();
				exit(1);
			}
			*(p++) = 0;	/* ends hostname */
			NextProxyPort = atoi(p);
			strcpy(NextProxyAdr, optarg);
			ConnectToProxy = 1;
			break;
		case 't':	/* disconnect time-out */
			timeout_out = atoi(optarg);
			break;
		case 'n':	/* no cache */
			NoCache = 1;
			break;
		case 'm':
			max_proxy_threads = atoi(optarg);
			break;
		}
	}

	free_q = nullptr;

	server(listenport);

	return 0;
}

