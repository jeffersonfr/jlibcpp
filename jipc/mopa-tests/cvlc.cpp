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
#include "jremoteipcclient.h"
#include "jremoteipcserver.h"
#include "jipcexception.h"
#include "jmethod.h"
#include "jstringtokenizer.h"

#include <iostream>

#include <stdio.h>
#include <strings.h>
#include <unistd.h>

std::string _addr;
int _port;

void usage(char *name)
{
	printf("%s client <addr> <port>\n", name);

	exit(0);
}

bool command(jipc::Method *method)
{
	jipc::RemoteIPCClient client(_addr, _port);
	jipc::Response *response = NULL;
	
	client.CallMethod(method, &response);
	
	if (response == NULL || response->GetBooleanParam("self") == false) {
		return false;
	}

	std::cout << response->What() << std::endl;

	return true;
}

bool remote_open(std::string path) 
{
	std::cout << "open { path: " << path << " }" << std::endl;

	jipc::Method method("open");

	method.SetTextParam("path", path);

	return command(&method);
}

bool remote_play()
{
	std::cout << "" << std::endl;

	jipc::Method method("play");

	return command(&method);
}

bool remote_pause()
{
	std::cout << "" << std::endl;

	jipc::Method method("pause");

	return command(&method);
}

bool remote_stop()
{
	std::cout << "" << std::endl;

	jipc::Method method("stop");

	return command(&method);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		usage(argv[0]);
	}

	_addr = argv[1];
	_port = atoi(argv[2]);

	try {
		remote_open("/tmp/shark.avi");
  
    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_play();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_pause();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_play();
    
    std::this_thread::sleep_for(std::chrono::seconds((2));
		
    remote_stop();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_open("/tmp/shark.avi");

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_play();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_pause();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_play();

    std::this_thread::sleep_for(std::chrono::seconds((2));

		remote_stop();
	} catch (jipc::IPCException &e) {
		std::cout << "Connection broken" << std::endl;
	}

	return 0;
}
