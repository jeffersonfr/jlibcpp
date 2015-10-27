#include "jthread.h"
#include "jspinlock.h"

#include <list>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/time.h>

#define SPINLOCK

#define LOOPS 10000000

std::list<int> the_list;

#ifdef SPINLOCK
jthread::SpinLock mutex;
#else
jthread::Mutex mutex;
#endif

class Consumer : public jthread::Thread {

	private:

	public:
		Consumer()
		{
		}

		virtual ~Consumer()
		{
		}

		virtual void Run()
		{
			printf("Consumer TID %lu\n", (unsigned long)syscall( __NR_gettid ));

			while (1) {
				mutex.Lock();

				if (the_list.empty()) {
					mutex.Unlock();

					break;
				}

				// the_list.front();
				the_list.pop_front();

				mutex.Unlock();
			}
		}

};
 
int main()
{
	Consumer consumer1,
					 consumer2;
	struct timeval tv1, tv2;

	for (int i = 0; i < LOOPS; i++) {
		the_list.push_back(i);
	}

	gettimeofday(&tv1, NULL);

	consumer1.Start();
	consumer2.Start();

	consumer1.WaitThread();
	consumer2.WaitThread();

	gettimeofday(&tv2, NULL);

	if (tv1.tv_usec > tv2.tv_usec) {
		tv2.tv_sec--;
		tv2.tv_usec += 1000000;
	}

	printf("Result: %ld.%ld\n", tv2.tv_sec - tv1.tv_sec, tv2.tv_usec - tv1.tv_usec);

	return 0;
}

