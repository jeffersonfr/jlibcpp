/***************************************************************************
 *   Copyright (C) 2010 by Jeff Ferr                                       *
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
#include "jipc/jremoteipcclient.h"
#include "jipc/jremoteipcserver.h"
#include "jipc/jmethod.h"
#include "jcommon/jstringtokenizer.h"

#include <iostream>

#include <stdio.h>
#include <strings.h>

bool alive = true;

class Callback : public jipc::RemoteCallListener {

	public:
		Callback()
		{
		}

		virtual ~Callback()
		{
		}

		virtual jipc::Response * ProcessCall(jipc::Method *method)
		{
			jipc::Response *response = new jipc::Response();
			
			response->SetBooleanParam("self", false);

			std::cout << "Server receive [" << method->What() << "]" << std::endl;

			if (method->GetName() == "quit") {
				alive = false;
			} else if (method->GetName() == "isPlaying") {
				response->SetBooleanParam("self", true);
			}

			std::cout << "Server response [" << response->What() << "]" << std::endl;

			return response;
		}

};

void usage(char *name)
{
	printf("%s server <port>\n", name);
	printf("%s client <ip> <port> <method> <param=value> ... <param=value>\n", name);

	exit(0);
}

void server(int port)
{
	jipc::RemoteIPCServer server(port);
	Callback cb;

	while (alive == true) {
		server.WaitCall(&cb);
	}
				
	std::cout << "bye ..." << std::endl;
}

void client(std::string ip, int port, jipc::Method *method)
{
	jipc::RemoteIPCClient client(ip, port);
	jipc::Response *response = nullptr;

	std::cout << "Client request [" << method->What() << "]" << std::endl;

	client.CallMethod(method, &response);

	std::cout << "Server response [" << response->What() << "]" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		usage(argv[0]);
	}

	if (strcasecmp(argv[1], "server") == 0) {
		server(atoi(argv[2]));
	} else if (strcasecmp(argv[1], "client") == 0) {
		jipc::Method method(argv[4]);

		for (int i=5; i<argc; i++) {
			std::string param = std::string(argv[i]);
			std::string::size_type r = param.find("=");

			if (r != std::string::npos) {
				method.SetTextParam(param.substr(0, r), param.substr(r+1));
			}
		}

		client(argv[2], atoi(argv[3]), &method);
	}

	return 0;
}
