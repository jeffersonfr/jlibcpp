#include "jmessagequeue.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace jshared;

// a estrutura naum pode conter ponteiros
typedef struct {
	char phone[20];
	char callbk[20];
} Page;

static char *cmdopt_s = 0;
static int cmdopt_p = 20;
static MessageQueue *msqid = NULL;

void usage() 
{
	puts("client use: ./msg_teste -k <key> [-p <priority>]");
	puts("server use: ./msg_teste -s <tty> -k <key>");

	exit(1);
}

void server()
{
	Page req;

	for (;;) {
		msqid->Receive(&req, sizeof(Page), -100L);

		if (!strcasecmp(req.phone, "shutdown")) {
			break;
		}
		
		printf("ATDT %s %s\n", req.phone, req.callbk);

		sleep(2);
	}

	msqid->Release();
}

void client(int argc, char **argv)
{
	Page req;

	strcpy(req.phone, "3244-4780");
	strcpy(req.callbk, "Jeff Ferr Call Back");

	msqid->Send(&req, sizeof(Page));
}

int main(int argc, char **argv)
{
	int rc = 0;
	int optch;
	key_t cmdopt_k = 0;
	const char cmdopts[] = "s:k:p:h";

	while ((optch = getopt(argc, argv, cmdopts)) != -1) {
		switch (optch) {
			case 'h':
				usage();
				break;
			case 's':
				cmdopt_s = optarg;
				break;
			case 'k':
				cmdopt_k = (key_t)atoi(optarg);
				break;
			case 'p':
				cmdopt_p = atoi(optarg);
				if (cmdopt_p < 1) {
					cmdopt_p = 1;
				} else if (cmdopt_p > 100) {
					cmdopt_p = 100;
				}
				break;
			default:
				rc = 1;
		}
	}

	if (rc) {
		exit(1);
	}

	if (cmdopt_s && optind < argc) {
		puts("Server mode does not accept args\n");
		usage();
	}

	if (!cmdopt_k) {
		puts("Must supply IPC key in -k option\n");
		usage();
	}

	if (cmdopt_s) {
		msqid = new MessageQueue(cmdopt_k, 0666);
		server();
	} else {
		msqid = new MessageQueue(cmdopt_k);
		client(argc, argv);
	}
	
	return 0;
}
