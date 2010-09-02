#include "jtimer.h"

#include <iostream>

class TaskTest : public jthread::TimerTask {

	private:

	public:
		TaskTest()
		{
		}
		
		virtual ~TaskTest()
		{
			std::cout << "Task Destructor" << std::endl;
		}

		void Run() 
		{
			std::cout << "Task running ..." << std::endl;
		}

};

int main() 
{
	jthread::Timer timer;
	TaskTest task;
	
	// wait 500000us to start task
	timer.Schedule(&task, 500000LL, 1000000LL, false);

	jthread::Thread::Sleep(5);

	return 0;
}

