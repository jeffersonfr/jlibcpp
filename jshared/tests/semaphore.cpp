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
#include "jshared/jsharedsemaphore.h"

#include <iostream>
#include <mutex>
#include <thread>

#include <sys/types.h>
#include <stdlib.h>

#define N	4

jshared::SharedSemaphore s(JIPC_PRIVATE, 1);

static std::mutex mutex;

class Test {

	private:
    std::thread _thread;
		int id;

	public:
		Test(int id)
		{
			this->id = id;
		}

		virtual ~Test()
		{
      _thread.join();
		}

		static void Log(int id, std::string str)
		{
			mutex.lock();
			
			std::cout << "[" << id << "]: " << str << std::flush << std::endl;
			
			mutex.unlock();
		}

		virtual void Start()
    {
      _thread = std::thread(&Test::Run, this);
    }

		virtual void Run()
		{
			Log(id, "Enter");

			s.At(0).Wait();

			Log(id, "Release");
		}
};

int main()
{
	Test *test[N];

	for (int i=0; i<N; i++) {
		test[i] = new Test(i);

		test[i]->Start();
	}

	sleep(2);

	s.At(0).NotifyAll();

	for (int i=0; i<N; i++) {
    Test *t = test[i];

		delete test[i];
	}

	return 0;
}

