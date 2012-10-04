/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

		virtual void Run() 
		{
			std::cout << "Task running ..." << std::endl;
		}

};

int main() 
{
	jthread::Timer timer;
	TaskTest task;
	
	timer.Start();

	// wait 500000us to start task
	timer.Schedule(&task, 500000LL, 1000000LL, false);

	jthread::Thread::Sleep(5);

	timer.Stop();

	return 0;
}

