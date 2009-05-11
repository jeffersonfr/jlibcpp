#include "jcondition.h"
#include "jsemaphore.h"
#include "jmutex.h"
#include "jthread.h"
#include "jthreadgroup.h"
#include "jthreadexception.h"

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
// jthread::Condition sem;

class IncCount : public jthread::Runnable {
	private:
		int id;

	public:
		IncCount(int i)
		{
			id = i;
		}
		
		~IncCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Routine() {
			int i;

			for (i=0; i<TCOUNT; i++) {
				monitor.Lock();

				count++;

				if (count == COUNT_LIMIT) {
					sem.Notify();
				}
					
				printf("inc: thread %d, count %d, unlock mutex.\n", id, count);

				monitor.Unlock();

			}
		}

};

class WatchCount : public jthread::Runnable {
	private:
		int id, c;

	public:
		WatchCount(int i)
		{
			id = i;
			c = 0;
		}

		~WatchCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Routine() {
			printf("Starting watch count: thread %d\n", id);
			
			while (count < COUNT_LIMIT) {
				sem.Wait(10000000LL);

				printf("Watch count: thread %d, count %d, condition signal received.\n", id, count);
			}
		}
		
};

int main() 
{
	try {
		jthread::ThreadGroup group1(3);

		IncCount inc1(thread_ids[0]), 
				 inc2(thread_ids[1]);
		WatchCount watch1(thread_ids[2]);

		group1.AttachThread(&watch1);
		group1.AttachThread(&inc1);
		group1.AttachThread(&inc2);

		group1.WaitForAll();
	} catch (...) {
		puts("...");
	}
	
	return 0;
}

