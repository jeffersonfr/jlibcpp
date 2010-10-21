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

