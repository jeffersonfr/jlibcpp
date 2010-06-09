#include "jthreadlib.h"

#include <iostream>

#define BUFFER_SIZE		1024

class Buffer {

	private:
		jthread::Mutex _lock;
		jthread::Condition _empty,
			_full;
		char **_buffer;
		int _index,
				_buffer_size,
				*_buffer_index;

	public:
		Buffer(int size = BUFFER_SIZE)
		{
			_buffer_size = size;

			_buffer = new char*[size];
			_buffer_index = new int[size];

			Reset();
		}

		virtual ~Buffer()
		{
		}

		void Reset()
		{
			_index = -1;
		}

		void Put(char *buffer, int size)
		{
			jthread::AutoLock l(&_lock);

			while (_index == _buffer_size) {
				_full.Wait(&_lock);
			}

			_index++;

			_buffer[_index] = buffer;
			_buffer_index[_index] = size;

			_empty.Notify();
		}

		void Get(char **buffer, int *size)
		{
			jthread::AutoLock l(&_lock);

			while (_index < 0) {
				_empty.Wait(&_lock);
			}

			memcpy(*buffer, _buffer[_index], _buffer_index[_index]);

			*size = _buffer_index[_index];

			_index--;

			_full.Notify();
		}

};

Buffer buffer;

class Producer : public jthread::Thread {

	public:
		Producer()
		{
		}

		virtual ~Producer()
		{
		}

		virtual void Run()
		{
			std::string s = "Jeff";

			for (int n = 0; n < 1000000; n++) {
		  		buffer.Put((char *)s.c_str(), s.size());
			}

			buffer.Put(NULL, 0);
		}

};

class Consumer : public jthread::Thread {

	public:
		Consumer()
		{
		}

		virtual ~Consumer()
		{
		}

		virtual void Run()
		{
			char *ptr = new char[BUFFER_SIZE];
			int size = 0;
			
			do {
				buffer.Get(&ptr, &size);
			
				std::cout << ptr << std::flush;
			} while (ptr != NULL);

			delete ptr;

			std::cout << "[over]" << std::endl;
		}

};

int main()
{
	Producer p;
	Consumer c;

	p.Start();
	c.Start();

	p.WaitThread();
	c.WaitThread();

	return 0;
}

