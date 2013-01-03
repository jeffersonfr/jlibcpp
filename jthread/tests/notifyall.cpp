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
#include "jthread.h"
#include "jsemaphore.h"
#include "jcondition.h"
#include "jmutex.h"

#include <iostream>

#define MAX_SEMAPHORES	10

// jthread::Semaphore sem;
jthread::Condition sem;
int sem_value = 10;

class NotifyAllSemaphore : public jthread::Thread {

	private:
		int _id;

	public:
		NotifyAllSemaphore(int id)
		{
			_id = id;
		}
		
		virtual ~NotifyAllSemaphore()
		{
		}

		virtual void Run() 
		{
			while (sem_value > 0) {
				sem.Wait();
			}
					
			std::cout << "Thread " << _id << " released." << std::endl;
		}

};

int main() 
{
	std::vector<NotifyAllSemaphore *> semaphores;

	for (int i=0; i<MAX_SEMAPHORES; i++) {
		NotifyAllSemaphore *nas = new NotifyAllSemaphore(i);

		nas->Start();

		semaphores.push_back(nas);
	}

	while (sem_value > 0) {
		sem_value = sem_value - 1;

		sem.NotifyAll();

		std::cout << "NotifyAll:: " << sem_value << std::endl;

		sleep(1);
	}
	
	for (int i=0; i<MAX_SEMAPHORES; i++) {
		NotifyAllSemaphore *nas = semaphores[i];

		delete nas;
	}

	semaphores.clear();

	return 0;
}

