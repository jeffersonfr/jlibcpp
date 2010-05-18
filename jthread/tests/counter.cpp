#include "jcondition.h"
#include "jsemaphore.h"
#include "jmutex.h"
#include "jthread.h"
#include "jthreadpool.h"
#include "jthreadexception.h"

#include <iostream>

#ifdef _WIN32
#include <winbase.h>
#endif

#include <unistd.h>
#include <stdio.h>

#define NUM_THREADS	3
#define TCOUNT		10
#define COUNT_LIMIT	12

int count = 0;
bool init = false;
int thread_ids[3] = {0, 1, 2};

jthread::Mutex monitor;
jthread::Semaphore sem;

class IncCount : public jthread::Thread {
	private:
		int id;

	public:
		IncCount(int i)
		{
			id = i;
		}
		
		virtual ~IncCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Run() {
			int i;

			for (i=0; i<TCOUNT; i++) {
				monitor.Lock();

				count++;

				if (count == COUNT_LIMIT) {
					sem.Notify();
				}
					
				std::cout << "inc: thread " << id << ", count " << count << ", unlock mutex." << std::endl;

				monitor.Unlock();

			}
		}

};

class WatchCount : public jthread::Thread {
	private:
		int id, c;

	public:
		WatchCount(int i)
		{
			id = i;
			c = 0;
		}

		virtual ~WatchCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Run() {
			std::cout << "Starting watch count: thread " << id << std::endl;
			
			while (count < COUNT_LIMIT) {
				sem.Wait(10000000LL);

				std::cout << "Watch count: thread " << id << ", count " << count << ", condition signal received." << std::endl;
			}
		}
		
};

int main() 
{
	try {
		IncCount inc1(thread_ids[0]), 
				 inc2(thread_ids[1]);
		WatchCount watch1(thread_ids[2]);

		watch1.Start();
		inc1.Start();
		inc2.Start();

		watch1.WaitThread();
		inc1.WaitThread();
		inc2.WaitThread();
	} catch (...) {
	}
	
	return 0;
}

