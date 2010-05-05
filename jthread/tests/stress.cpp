/**
 *	compile: g++ teste.cpp -o teste -L. -ljthread -lpthread
 */

#include "jcondition.h"
#include "jmutex.h"
#include "jsemaphore.h"
#include "jthread.h"
#include "jthreadexception.h"
#include "jdate.h"

#include <unistd.h>
#include <sys/time.h>

#define MAX_LOOP_THREAD		300
#define MAX_LOOP_SEMAPHORE	100000
#define MAX_LOOP_MUTEX		1000000

class T : public jthread::Thread{

	public:
		int i; 

		T():jthread::Thread() 
		{
			i = 10;
		}

		virtual ~T() 
		{
		}

		virtual void Start()
		{
			printf("Hello, world %d\n", i);
		}

};

long long get_current_time()
{
	return (long long)jcommon::Date::CurrentTimeMillis();
}

int main() {
	long long start, finish;
	int i;

	// Thread loop
	start = get_current_time();
	
	jthread::Thread t;

	try {
		for (i=0; i<MAX_LOOP_THREAD; i++) {
			t.Start();
			t.WaitThread();
		}
	} catch (...) {
		perror("Error:: ");
	}
	
	finish = get_current_time();
	
	printf("Thread loop: %.4f ms\n", (finish - start)/1000.0);
	
	/*
	// Semaphore loop
	start = get_current_time();
	
	for (i=0; i<MAX_LOOP_SEMAPHORE; i++) {
		jthread::Semaphore s;
	}
	
	finish = get_current_time();
	
	printf("Semaphore loop: %.4f ms\n", (finish - start)/1000.0);
	
	// Mutex loop
	start = get_current_time();
	
	jthread::Mutex m;

	for (i=0; i<MAX_LOOP_MUTEX; i++) {
		m.Lock();
		m.Unlock();
	}
	
	finish = get_current_time();
	
	printf("Mutex loop: %.4f ms\n", (finish - start)/1000.0);
	*/

	return 0;
}
