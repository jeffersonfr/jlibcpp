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
#include "jio/jbufferreader.h"
#include "jio/jbufferwriter.h"
#include "jnetwork/jsocket.h"
#include "jnetwork/jserversocket.h"

#include <iostream>
#include <thread>

#include <unistd.h>

#define PORT	1234

class Server {

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
      jnetwork::ServerSocket server(PORT);

      jnetwork::Socket *socket = server.Accept();

			char buffer[4096];
			int64_t buffer_size;

			buffer_size = socket->Receive(buffer, 4096);

			if (buffer_size > 0) {
				jio::BufferReader reader(buffer, buffer_size);

				bool boolean = reader.ReadBoolean();
				float decimal = reader.ReadFloat();
				std::string str = reader.ReadString();
				int integer = reader.ReadInteger();
				char *raw = reader.ReadRaw(&buffer_size);

				std::cout << "Server:: "
					<< "[" << (const char *)raw << "]" 
					"[" << integer << "]"
					"[" << str << "]"
					"[" << decimal <<  "]"
					"[" << boolean << "]"
					<<  std::endl;

				delete [] raw;

				jio::BufferWriter writer;

				writer.WriteBoolean(true);
				writer.WriteFloat(-1.0);
				writer.WriteString("testando 3 2 1 ...");
				writer.WriteInteger(-10);
				writer.WriteRaw("Jeff Ferr *", 11+1);
			
				socket->Send(writer.GetData().c_str(), writer.GetData().size());
			}

      socket->Close();

      delete socket;
      socket = nullptr;

      server.Close();
		}

};

class Client {

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
      jnetwork::Socket socket("127.0.0.1", PORT);

			jio::BufferWriter writer;

			writer.WriteBoolean(false);
			writer.WriteFloat(1.0);
			writer.WriteString("testando 1 2 3 ...");
			writer.WriteInteger(10);
			writer.WriteRaw("* Jeff Ferr", 11+1);

			socket.Send(writer.GetData().c_str(), writer.GetData().size());

			char buffer[4096];
			int64_t buffer_size;

			buffer_size = socket.Receive(buffer, 4096);

			if (buffer_size > 0) {
				jio::BufferReader reader(buffer, buffer_size);
				
				bool boolean = reader.ReadBoolean();
				float decimal = reader.ReadFloat();
				std::string str = reader.ReadString();
				int integer = reader.ReadInteger();
				char *raw = reader.ReadRaw(&buffer_size);

				std::cout << "Client:: "
					<< "[" << (const char *)raw << "]" 
					"[" << integer << "]"
					"[" << str << "]"
					"[" << decimal <<  "]"
					"[" << boolean << "]"
					<<  std::endl;

				delete [] raw;
			}

      socket.Close();
		}

};

int main()
{
	Server server;
	Client client;

  std::thread st(&Server::Run, &server);

  std::this_thread::sleep_for(std::chrono::seconds((1)));

  std::thread ct(&Client::Run, &client);

  ct.join();
  st.join();

	return 0;
}
