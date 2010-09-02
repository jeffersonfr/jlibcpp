#include "jmonitor.h"
#include "jthread.h"

#include <iostream>

#include <stdio.h>

jthread::Monitor cond;

class MonitorWaiterThread : public jthread::Thread {

	public:
		virtual void Run()
		{
			cond.Enter();
			cond.Enter();
			cond.Wait();

			std::cout << "MonitorWaiterThread woke up after wait()" << std::endl;
			
			cond.Exit();
			cond.Exit();
		}

};

class MonitorTimedWaiterThread : public jthread::Thread {

	public:
		virtual void Run()
		{
			cond.Enter();
			cond.Wait(5000000LL);
			printf("MonitorTimedWaiterThread woke up after wait()\n");
			cond.Exit();
		}

};

class MonitorNotifierThread : public jthread::Thread {

	public:
		virtual void Run()
		{
			jthread::Thread::Sleep(10LL);
			printf("Notifying all threads\n");
			cond.NotifyAll();
		}

};

int main()
{
	jthread::Thread *t1 = new MonitorWaiterThread(),
				 *t2 = new MonitorTimedWaiterThread(),
				 *t3 = new MonitorNotifierThread();

	t1->Start();
	
	jthread::Thread::Sleep(1LL);
	
	t2->Start();
	t3->Start();

	t1->WaitThread();
	t2->WaitThread();
	t3->WaitThread();

	jthread::Thread::Sleep(10LL);

	return 0;
}	

