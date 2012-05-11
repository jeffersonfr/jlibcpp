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
#include "jdate.h"

#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define MAX_LOOP_THREAD		1000
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

		virtual void Run()
		{
			std::cout << "Hello, world !" << std::endl;
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
	
	T t;

	try {
		for (i=0; i<MAX_LOOP_THREAD; i++) {
			t.Start();
			t.WaitThread();
		}
	} catch (...) {
		printf("Thread count:: %d, error:: %s\n", i, strerror(errno));
	}

	t.Release();
	
	finish = get_current_time();
	
	std::cout << "Thread loop: " << (finish - start)/1000.0 << " ms" << std::endl;
	
	/*
	// Semaphore loop
	start = get_current_time();
	
	for (i=0; i<MAX_LOOP_SEMAPHORE; i++) {
		jthread::Semaphore s;
	}
	
	finish = get_current_time();
	
	std::cout << "Semaphore loop: " << (finish - start)/1000.0 << " ms" << std::endl;
	
	// Mutex loop
	start = get_current_time();
	
	jthread::Mutex m;

	for (i=0; i<MAX_LOOP_MUTEX; i++) {
		m.Lock();
		m.Unlock();
	}
	
	finish = get_current_time();
	
	std::cout << "Mutex loop: " << (finish - start)/1000.0 << " ms" << std::endl;
	*/

	return 0;
}
