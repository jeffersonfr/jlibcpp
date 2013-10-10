#include "jsharedsemaphore.h"
#include "jthread.h"
#include "jmutex.h"

#include <iostream>

#include <sys/types.h>
#include <stdlib.h>

#define N	4

jshared::SharedSemaphore s(JIPC_PRIVATE, 1);

class Test : public jthread::Thread {

	private:
		static jthread::Mutex mutex;

		int id;

	public:
		Test(int id)
		{
			this->id = id;
		}

		virtual ~Test()
		{
		}

		static void Log(int id, std::string str)
		{
			mutex.Lock();
			
			std::cout << "[" << id << "]: " << str << std::flush << std::endl;
			
			mutex.Unlock();
		}

		virtual void Run()
		{
			Log(id, "Enter");

			s.At(0).Wait();

			Log(id, "Release");
		}
};

jthread::Mutex Test::mutex;

int main()
{
	Test *test[N];

	for (int i=0; i<N; i++) {
		test[i] = new Test(i);
		test[i]->Start();
	}

	sleep(2);

	s.At(0).NotifyAll();

	for (int i=0; i<N; i++) {
		test[i]->WaitThread();
	}

	return 0;
}

