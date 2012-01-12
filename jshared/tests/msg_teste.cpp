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
#include "jmessagequeue.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

using namespace jshared;

// WARNNING:: a estrutura naum pode conter ponteiros
typedef struct {
	char phone[20];
	char callbk[20];
} Page;

static char *cmdopt_s = 0;
static int cmdopt_p = 20;
static MessageQueue *msqid = NULL;

void usage() 
{
	std::cout << "client use: ./msg_teste -k <key> [-p <priority>]" << std::endl;
	std::cout << "server use: ./msg_teste -s <tty> -k <key>" << std::endl;

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
		
		std::cout << "ATDT " << req.phone << " " << req.callbk << std::endl;

		sleep(2);
	}

	msqid->Release();
	
	delete msqid;
}

void client(int argc, char **argv)
{
	Page req;

	strcpy(req.phone, "3244-4780");
	strcpy(req.callbk, "Jeff Ferr Call Back");

	msqid->Send(&req, sizeof(Page));

	delete msqid;
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
		std::cout << "Server mode does not accept args\n" << std::endl;

		usage();
	}

	if (!cmdopt_k) {
		std::cout << "Must supply IPC key in -k option\n" << std::endl;
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
