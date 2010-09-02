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

#include "jserversocket.h"
#include "jsocket.h"
#include "jsocketexception.h"
#include "jthreadlib.h"
#include "jthread.h"
#include "jsemaphoretimeoutexception.h"
#include "jautolock.h"

#include <iostream>

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
	jthread::Mutex mu;
	jthread::Condition cond;
	jsocket::Socket *sock_in;
};

// global variables
jthread::Mutex free_q_mu;
jthread::Condition free_q_cond;
struct th_proxy_struct *free_q;
int thread_count = 0;		// protected with free_q_mu 

// functions prototype
void help();
struct th_proxy_struct *new_proxy_th(void);
struct th_proxy_struct *alloc_proxy_th(void);
void server(int port);
void sayerror(char *msg, jsocket::Socket *sockIn, jsocket::Socket *sockOut);
int process_request(jsocket::Socket *sockIn);

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

class Client : public jthread::Thread{

	private:
		struct th_proxy_struct *th_proxy;

	public:
		Client(struct th_proxy_struct *th)
		{
			th_proxy = th;
		}

		virtual ~Client()
		{
		}

		virtual void Run()
		{
			struct timespec ts;
			struct timeval tv;
			struct th_proxy_struct **th;

			signal(SIGPIPE, SIG_IGN);
			for (;;) {
				th_proxy->mu.Lock();

				while (th_proxy->sock_in == NULL) {
					gettimeofday(&tv, NULL);
					ts.tv_sec = tv.tv_sec + TIMEOUT_THREAD_EXIT;
					ts.tv_nsec = 0;
					
					try {
						th_proxy->cond.Wait((long long)ts.tv_sec*1000000LL + (long long)ts.tv_nsec/1000LL, &th_proxy->mu);
					} catch (jthread::SemaphoreTimeoutException &e) {
						free_q_mu.Lock();
						th = &free_q;
						while (*th && *th != th_proxy)
							th = &((*th)->next_free);
						if (*th == th_proxy) {
							// remove yourself from queue and exit
							*th = th_proxy->next_free;
							thread_count--;
							free_q_mu.Unlock();
							free(th_proxy);
							goto run_exit;
						}
						free_q_mu.Unlock();
					}
				}

				th_proxy->mu.Unlock();

				process_request(th_proxy->sock_in);

				th_proxy->mu.Lock();
				th_proxy->sock_in = NULL;
				th_proxy->mu.Unlock();

				free_q_mu.Lock();
				th_proxy->next_free = free_q;
				free_q = th_proxy;
				free_q_mu.Unlock();
				free_q_cond.Notify();
			}

			run_exit:
				return;
		}
};

struct th_proxy_struct *new_proxy_th(void)
{
	struct th_proxy_struct *th_proxy = NULL;

	try {
		th_proxy = new struct th_proxy_struct;

		memset(th_proxy, 0, sizeof(struct th_proxy_struct));

		th_proxy->next_free = NULL;
		th_proxy->sock_in = NULL;

		Client *client = new Client(th_proxy);

		client->Start();
	} catch (std::bad_alloc &e) {
		if ((void *)th_proxy != NULL) {
			delete th_proxy;
		}

		return NULL;
	}

	return th_proxy;
}

struct th_proxy_struct *alloc_proxy_th(void)
{
	struct th_proxy_struct *th_proxy;

	jthread::AutoLock l(&free_q_mu);

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
				free_q_cond.Wait(&free_q_mu);
			}
		}
	} while (!th_proxy);

	return th_proxy;
}

void server(int port)
{
	struct th_proxy_struct *th_proxy;

	jsocket::ServerSocket server(port);
	jsocket::Socket *s;

	for (;;) {
		try {
			s = server.Accept();
		
			th_proxy = alloc_proxy_th();

			th_proxy->mu.Lock();
			th_proxy->sock_in = s;
			th_proxy->mu.Unlock();

			th_proxy->cond.Notify();
		} catch (jsocket::SocketException &e) {
			perror("Broken connection");
		}
	}
}

/* this function should be called only by the child */
void sayerror(char *msg, jsocket::Socket *sockIn, jsocket::Socket *sockOut)
{
	sockIn->Send(msg, strlen(msg));
	
	jsocket::SocketOption *o = sockIn->GetSocketOption();

	o->SetLinger(1, 4);

	delete o;

	o = sockOut->GetSocketOption();

	o->SetLinger(1, 1);

	delete o;

	sockOut->Close();
	sockIn->Close();
}

int process_request(jsocket::Socket *sockIn)
{
	char data[LDATA];
	char adr[LADR], *p;
	int ldata, lreq, port, req_len, req_method;
	jsocket::Socket *sockOut = NULL;
	FILE *fsin;
	
	jsocket::SocketOption *o = sockIn->GetSocketOption();

	o->SetReuseAddress(true);
	o->SetKeepAlive(true);

	delete o;

	if ((fsin = fdopen(sockIn->GetHandler(), "rw")) == NULL) {
		goto serverr;
	}

	// here, we'll analyze the request and get rid of "http://adr:port". The address and port willbe duplicated and used to open the connection
	if (fgets(data, LDATA, fsin) == NULL) {
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
		if ((p = strchr(str, '/')) != NULL) {
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
		if (p == NULL)	/* unspecified port. The default one will be used */
			port = DEFAULTPORT;
		else {		/* port is available. let's read it */
			*(p++) = 0;	/* ends hostname */
			port = atoi(p);
		}
		/* end of request analysis. The hostname is in "adr", and the port in "port" */
		try {
			sockOut = new jsocket::Socket(adr, port);
		} catch (jsocket::SocketException &e) {
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
			sockOut = new jsocket::Socket(NextProxyAdr, NextProxyPort);
		} catch (jsocket::SocketException &e) {
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
			fgets(data, LDATA, fsin);
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
			ldata = fread(data, 1, (LDATA > c_len ? c_len : LDATA), fsin);
			sockOut->Send(data, ldata);
			c_len -= ldata;
		}
	} else { /*
		  * METHOD_GET, METHOD_HEAD
		  */
		do {
			fgets(data, LDATA, fsin);
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
			if (p == NULL) {	/* unspecified port number. let's quit */
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

	free_q = NULL;

	server(listenport);

	return 0;
}

