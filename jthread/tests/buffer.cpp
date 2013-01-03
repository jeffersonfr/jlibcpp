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
#include "jindexedbuffer.h"
#include "jsocket.h"
#include "jthread.h"

#define MAX_CLIENTS		1000

class Source {

	private:
		jthread::IndexedBuffer *_buffer;
		int _packet_index;
		int _packet_max;
		char *_packet;
		bool _flag;

	public:
		Source(jthread::IndexedBuffer *b)
		{
			_buffer = b;

			_flag = true;
			_packet_index = 0;
			_packet_max = 0;

			_packet = new char[188*7];
		}

		virtual ~Source()
		{
		}

		int read(char *data, int size)
		{
			jthread::jbuffer_chunk_t t;
			int r, 
					d = _packet_max - _packet_index;

			if (d > 0) {
				if (size <= d) {
					memcpy(data, (_packet + _packet_index), size);

					_packet_index = _packet_index + size;

					return size;
				}

				memcpy(data, (_packet + _packet_index), d);
			}

			_packet_index = 0;
			_packet_max = 0;

			r = _buffer->Read(&t);

			if (r < 0) {
				_buffer->GetIndex(&t);
			} else {
				memcpy(_packet, t.data, t.size);

				_packet_max = t.size;

				if (_packet_max > 0) {
					if (size <= _packet_max) {
						memcpy(data, _packet, size);

						_packet_index = size;

						return size;
					}

					memcpy(data, _packet, _packet_max);

					_packet_max = 0;

					return size;
				}
			}

			return -1;
		}

};

class Manager : public jthread::Thread {

	private:
		static Manager *_instance;

		jthread::IndexedBuffer *_buffer;

	private:
		Manager()
		{
			_buffer = new jthread::IndexedBuffer(4096, 188*7);
		}

	public:
		virtual ~Manager()
		{
			delete _buffer;
		}

		static Manager * GetInstance()
		{
			return _instance;
		}

		Source * CreateSource() 
		{
			return new Source(_buffer);
		}

		virtual void Run() 
		{
			jsocket::Socket socket("127.0.0.1", 1234);

			uint8_t buf[4096];
			int size,
					max = 7*188;

			while (true) {
				size = socket.Receive((char *)buf, max);

				if (size == max) {
					_buffer->Write(buf, max);
				}
			}
		}

};

class Client : public jthread::Thread {

	private:
		Source *_source;

	public:
		Client()
		{
			_source = Manager::GetInstance()->CreateSource();
		}

		virtual ~Client()
		{
			delete _source;
		}

		virtual void Run()
		{
			// do anything with _source
		}

};

Manager *Manager::_instance = new Manager();

int main()
{
	std::vector<Client *> clients;

	// initialize media bank
	Manager::GetInstance()->Start();

	// initialize media client
	for (int i=0; i<MAX_CLIENTS; i++) {
		Client *client = new Client();

		client->Start();

		clients.push_back(client);
	}

	// wait all clients
	for (int i=0; i<MAX_CLIENTS; i++) {
		Client *client = clients[i];

		client->WaitThread();
	}

	return 0;
}

