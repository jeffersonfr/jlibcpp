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
#include "jsocket.h"
#include "jsocketexception.h"
#include "junknownhostexception.h"
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
			
			std::cout << " [ " << (jcommon::Date::CurrentTimeMicros()-t1) << " us ]" << std::endl;
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
		}

		virtual void InitClient()
		{
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
			server = new LocalServerSocket("/tmp/ltcp.server");
		}

		virtual ~lTCPTest()
		{
		}

		virtual void InitServer()
		{
			client = server->Accept();

			Start();
		}

		virtual void InitClient()
		{

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
		}

		virtual void InitClient()
		{
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
		}

		virtual void InitClient()
		{
		}

};

int main()
{
	ConnectionTest *itcp = new iTCPTest(),
								 *ltcp = new lTCPTest(),
								 *iudp = new iUDPTest(),
								 *ludp = new lUDPTest();

	itcp->Init();
	ltcp->Init();
	iudp->Init();
	ludp->Init();

	return 0;
}

