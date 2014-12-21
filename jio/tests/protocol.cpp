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
#include "jbufferreader.h"
#include "jbufferwriter.h"
#include "jthread.h"
#include "jsocket.h"
#include "jserversocket.h"

#include <iostream>

#include <unistd.h>

#define PORT	1234

class Server : public jthread::Thread {

	private:

	public:
		Server()
		{
		}

		virtual ~Server()
		{
		}

		virtual void Run()
		{
			jsocket::ServerSocket server(PORT);

			jsocket::Socket *socket = server.Accept();

			char buffer[4096];
			int buffer_size;

			buffer_size = socket->Receive(buffer, 4096);

			if (buffer_size > 0) {
				jio::BufferReader reader((uint8_t *)buffer, buffer_size);

				std::cout << "Server:: Receive:: "
					<< "[" << (const char *)reader.ReadRaw(&buffer_size) << "]" 
					<< "[" << reader.ReadInteger() << "]"
					<< "[" << reader.ReadString() << "]"
					<< "[" << reader.ReadFloat() <<  "]"
					<< "[" << reader.ReadBoolean() << "]"
					<<  std::endl;

				jio::BufferWriter writer;

				writer.WriteBoolean(true);
				writer.WriteFloat(20.0);
				writer.WriteString("testando 1 2 3 ...");
				writer.WriteInteger(30);
				writer.WriteRaw((uint8_t *)"Jeff Ferr", 9+1);
			
				socket->Send(writer.GetData().c_str(), writer.GetData().size());
			}
		}

};

class Client : public jthread::Thread {

	private:

	public:
		Client()
		{
		}

		virtual ~Client()
		{
		}

		virtual void Run()
		{
			jsocket::Socket socket("127.0.0.1", PORT);

			jio::BufferWriter writer;

			writer.WriteBoolean(true);
			writer.WriteFloat(2.0);
			writer.WriteString("testando 1 2 3 ...");
			writer.WriteInteger(3);
			writer.WriteRaw((uint8_t *)"Ferr Jeff", 9+1);

			socket.Send(writer.GetData().c_str(), writer.GetData().size());

			char buffer[4096];
			int buffer_size;

			buffer_size = socket.Receive(buffer, 4096);

			if (buffer_size > 0) {
				jio::BufferReader reader((uint8_t *)buffer, buffer_size);
				
				std::cout << "Client:: Receive:: "
					<< "[" << (const char *)reader.ReadRaw(&buffer_size) << "]" 
					"[" << reader.ReadInteger() << "]"
					"[" << reader.ReadString() << "]"
					"[" << reader.ReadFloat() <<  "]"
					"[" << reader.ReadBoolean() << "]"
					<<  std::endl;
			}
		}

};

int main()
{
	Server server;
	Client client;

	server.Start();

	sleep(1);

	client.Start();

	client.WaitThread();
	server.WaitThread();

	return 0;
}
