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

