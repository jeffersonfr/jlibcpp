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
#include "jnetwork/jdatagramsocket.h"
#include "jnetwork/jlocalserversocket.h"
#include "jnetwork/jlocalsocket.h"
#include "jnetwork/jlocaldatagramsocket.h"
#include "jnetwork/jsocketlib.h"
#include "jcommon/jdate.h"
#include "jexception/jexception.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

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
			jnetwork::ServerSocket server(12345);
			jnetwork::Socket *socket = server.Accept();

			char buffer[2048];

			try {
				while (socket->Receive(buffer, 1500) > 0) {
				}
			} catch (jexception::Exception &e) {
			}

			socket->Close();
			delete socket;

			server.Close();
		}

		virtual void InitClient()
		{
			jnetwork::Socket socket("localhost", 12345);

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
    jnetwork::LocalServerSocket *server;
		jnetwork::LocalSocket *client;

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
			jnetwork::LocalServerSocket server("/tmp/ltcp.server");
			jnetwork::LocalSocket *socket = server.Accept();

			char buffer[2048];

			try {
				while (socket->Receive(buffer, 1500) > 0) {
				}
			} catch (jexception::Exception &e) {
			}
		}

		virtual void InitClient()
		{
			jnetwork::LocalSocket socket("/tmp/ltcp.server");

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
			jnetwork::DatagramSocket server(54321);

			char buffer[2048];

			try {
				while (server.Receive(buffer, 1500) > 0) {
				}
			} catch (jexception::Exception &e) {
			}

			server.Close();
		}

		virtual void InitClient()
		{
			jnetwork::DatagramSocket socket("localhost", 54321);

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
			jnetwork::LocalDatagramSocket server("/tmp/ludp.server");

			char buffer[2048];

			try {
				while (server.Receive(buffer, 1500) > 0) {
				}
			} catch (jexception::Exception &e) {
			}

			server.Close();
		}

		virtual void InitClient()
		{
			jnetwork::LocalDatagramSocket socket("/tmp/ludp.client", "/tmp/ludp.server");

			char buffer[2048];
			int i = 0;

			while (i++ < 100000) {
				try {
				socket.Send(buffer, 1500);
				} catch (...) {
				}
			}
			
			socket.Close();
		}

};

int main()
{
	(new iUDPTest())->Init();
	(new lUDPTest())->Init();
	(new iTCPTest())->Init();
	(new lTCPTest())->Init();

	return 0;
}

