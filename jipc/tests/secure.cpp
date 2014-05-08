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
#include "jsecureipcclient.h"
#include "jsecureipcserver.h"
#include "jmethod.h"
#include "jstringtokenizer.h"
#include "jsocketlib.h"

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

			std::cout << "Server receive [" << method->what() << "]" << std::endl;

			if (method->GetName() == "quit") {
				alive = false;
			} else if (method->GetName() == "isPlaying") {
				response->SetBooleanParam("self", true);
			}

			std::cout << "Server response [" << response->what() << "]" << std::endl;

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
	jsocket::SSLContext *context = jsocket::SSLContext::CreateServerContext();
	
	// Load our certificate
	if (context->SetCertificate("certs/cert.pem", "certs/cert_key.pem", "qwerty") == false) {
		std::cout << "Load certificates failed ..." << std::endl;

		delete context;

		exit(1);
	}

	context->SetDHFile("certs/dh1024.pem");

	jipc::SecureIPCServer server(context, port);
	Callback cb;

	while (alive == true) {
		server.WaitCall(&cb);
	}
				
	delete context;

	std::cout << "bye ..." << std::endl;
}

void client(std::string ip, int port, jipc::Method *method)
{
	jsocket::SSLContext *context = jsocket::SSLContext::CreateClientContext("certs/cert.pem");
	jipc::SecureIPCClient client(context, ip, port);
	jipc::Response *response = NULL;

	std::cout << "Client request [" << method->what() << "]" << std::endl;

	client.CallMethod(method, &response);

	std::cout << "Server response [" << response->what() << "]" << std::endl;

	delete context;
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		usage(argv[0]);
	}

	InitializeSocketLibrary();
	
	if (strcasecmp(argv[1], "server") == 0) {
		server(atoi(argv[2]));
	} else if (strcasecmp(argv[1], "client") == 0) {
		jipc::Method method(argv[4]);

		for (int i=5; i<argc; i++) {
			jcommon::StringTokenizer tokens(std::string(argv[i]), "=", jcommon::JTT_STRING);

			if (tokens.GetSize() > 1) {
				std::string key = tokens.GetToken(0), 
					value = tokens.GetToken(1);

				if (key.size() > 0 && value.size() > 0) {
					method.SetTextParam(key, value);
					// method.SetTextParam(tokens.GetToken(0), tokens.GetToken(1));
				}
			}
		}

		client(argv[2], atoi(argv[3]), &method);
	}

	ReleaseSocketLibrary();

	return 0;
}
