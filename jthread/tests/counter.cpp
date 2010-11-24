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
#include "jmutex.h"
#include "jsemaphoretimeoutexception.h"

#include <iostream>

#define NUM_THREADS	3
#define TCOUNT		10
#define COUNT_LIMIT	12

int count = 0;
bool init = false;
int thread_ids[3] = {0, 1, 2};

jthread::Mutex monitor;
jthread::Semaphore sem;

class IncCount : public jthread::Thread {
	private:
		int id;

	public:
		IncCount(int i)
		{
			id = i;
		}
		
		virtual ~IncCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Run() {
			int i;

			for (i=0; i<TCOUNT; i++) {
				monitor.Lock();

				count++;

				if (count == COUNT_LIMIT) {
					sem.Notify();
				}
					
				std::cout << "inc: thread " << id << ", count " << count << ", unlock mutex." << std::endl;

				monitor.Unlock();

			}
		}

};

class WatchCount : public jthread::Thread {
	private:
		int id, c;

	public:
		WatchCount(int i)
		{
			id = i;
			c = 0;
		}

		virtual ~WatchCount()
		{
			if (monitor.IsLocked() == true) {
				monitor.Unlock();
			}
		}

		void Run() {
			std::cout << "Starting watch count: thread " << id << std::endl;
			
			while (count < COUNT_LIMIT+10) {
				try {
					sem.Wait(10*1000000LL);
				} catch (jthread::SemaphoreTimeoutException &e) {
					std::cout << "Watch count: thread " << id << ", count " << count << ", condition signal received." << std::endl;

					break;
				}
			}
		}
		
};

int main() 
{
	try {
		IncCount inc1(thread_ids[0]), 
				 inc2(thread_ids[1]);
		WatchCount watch1(thread_ids[2]);

		watch1.Start();
		inc1.Start();
		inc2.Start();

		watch1.WaitThread();
		inc1.WaitThread();
		inc2.WaitThread();
	} catch (...) {
	}
	
	return 0;
}

