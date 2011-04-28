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
#include "jdatagramsocket.h"
#include "jlocalserversocket.h"
#include "jlocalsocket.h"
#include "jlocaldatagramsocket.h"
#include "junknownhostexception.h"
#include "jsocketexception.h"
#include "jthread.h"
#include "jdate.h"
#include "jsocketlib.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

using namespace jsocket;
using namespace jio;
using namespace jthread;

class ConnectionTest : public Thread {

	private:
		std::string _label;

	public:
		ConnectionTest(std::string label)
		{
			_label = label;
		}

		virtual ~ConnectionTest()
		{
		}

		virtual void InitServer()
		{
		}

		virtual void InitClient()
		{
		}

		virtual void Init()
		{
			std::cout << "Iniciando server < " << _label << " >" << std::endl;

			Start();

			sleep(1);

			std::cout << "Iniciando client" << std::flush;

			uint64_t t1 = jcommon::Date::CurrentTimeMicros();

			InitClient();
			
			std::cout << " [ " << (jcommon::Date::CurrentTimeMicros()-t1) << " us ]\n" << std::endl;
		}

		virtual void Run()
		{
			InitServer();
		}

};

class iTCPTest : public ConnectionTest {

	private:

	public:
		iTCPTest():
			ConnectionTest("iTCP")
		{
		}

		virtual ~iTCPTest()
		{
		}

		virtual void InitServer()
		{
			jsocket::ServerSocket server(12345);
			jsocket::Socket *socket = server.Accept();

			char buffer[2048];

			while (socket->Receive(buffer, 1500) > 0) {
			}

			socket->Close();
			delete socket;

			server.Close();
		}

		virtual void InitClient()
		{
			jsocket::Socket socket("localhost", 12345);

			char buffer[2048];
			int i = 0;

			while (i++ < 100000) {
				socket.Send(buffer, 1500);
			}

			socket.Close();
		}

};

class lTCPTest : public ConnectionTest {

	private:
		LocalServerSocket *server;
		LocalSocket *client;

	public:
		lTCPTest():
			ConnectionTest("lTCP")
		{
		}

		virtual ~lTCPTest()
		{
		}

		virtual void InitServer()
		{
			jsocket::LocalServerSocket server("/tmp/ltcp.server");
			jsocket::LocalSocket *socket = server.Accept();

			char buffer[2048];

			while (socket->Receive(buffer, 1500) > 0) {
			}
		}

		virtual void InitClient()
		{
			jsocket::LocalSocket socket("/tmp/ltcp.server");

			char buffer[2048];
			int i = 0;

			while (i++ < 100000) {
				socket.Send(buffer, 1500);
			}

			socket.Close();
		}

};

class iUDPTest : public ConnectionTest {

	private:

	public:
		iUDPTest():
			ConnectionTest("iUDP")
		{
		}

		virtual ~iUDPTest()
		{
		}

		virtual void InitServer()
		{
			jsocket::DatagramSocket server(54321);

			char buffer[2048];

			while (server.Receive(buffer, 1500) > 0) {
			}

			server.Close();
		}

		virtual void InitClient()
		{
			jsocket::DatagramSocket socket("localhost", 54321);

			char buffer[2048];
			int i = 0;

			while (i++ < 100000) {
				socket.Send(buffer, 1500);
			}

			socket.Close();
		}

};

class lUDPTest : public ConnectionTest {

	private:

	public:
		lUDPTest():
			ConnectionTest("lUDP")
		{
		}

		virtual ~lUDPTest()
		{
		}

		virtual void InitServer()
		{
			jsocket::LocalDatagramSocket server("/tmp/ludp.server");

			char buffer[2048];

			while (server.Receive(buffer, 1500) > 0) {
			}

			server.Close();
		}

		virtual void InitClient()
		{
			jsocket::LocalDatagramSocket socket("/tmp/ludp.client", "/tmp/ludp.server");

			char buffer[2048];
			int i = 0;

			while (i++ < 100000) {
				socket.Send(buffer, 1500);
			}

			socket.Close();
		}

};

int main()
{
	(new lUDPTest())->Init();
	(new iTCPTest())->Init();
	(new lTCPTest())->Init();
	(new iUDPTest())->Init();

	return 0;
}

