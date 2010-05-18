#include "jmonitor.h"

#include <stdio.h>

jthread::Monitor cond;

class MonitorWaiter : public jthread::Thread {

	public:
		virtual void Run()
		{
			cond.Enter();
			cond.Wait();
			printf("MonitorWaiter woke up after wait()\n");
			cond.Exit();
		}

};

int main()
{
	jthread::Thread *t1 = new MonitorWaiter(),
		*t2 = new MonitorWaiter();

	t1->Start();
	t2->Start();

	jthread::Thread::Sleep(1);

	return 0;
}

