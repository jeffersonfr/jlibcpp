#include "jindexedbuffer.h"
#include "jsocket.h"

#define MAX_CLIENTS		1000

class Source {

	private:
		jthread::IndexedBuffer *_buffer;
		int _read_index;
		int _pass_index;
		int _packet_index;
		int _packet_max;
		char *_packet;
		bool _flag;

	public:
		Source(jthread::IndexedBuffer *b)
		{
			_buffer = b;

			_read_index = 0;
			_pass_index = 0;
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
			jthread::jringbuffer_t t;
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

			r = _buffer->Read(&t, &_read_index, &_pass_index);

			if (r < 0) {
				_buffer->GetIndex(&_read_index, &_pass_index);
			} else {
				memcpy(_packet, t.data, t.size);

				_packet_max = t.size;

				memcpy((data + d), _packet, (size - d));

				_packet_index = size - d;

				return size;
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

