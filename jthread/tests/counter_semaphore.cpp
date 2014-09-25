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
#include "jcondition.h"

#include <iostream>

#include <stdlib.h>

class CounterSemaphore {

	private:
		jthread::Mutex _mutex;
		jthread::Condition _condition;
		int _value;

	public:
		CounterSemaphore(int value = 0)
		{
			_value = value+1;
		}
		
		virtual ~CounterSemaphore()
		{
		}

		virtual void Wait() 
		{
			_mutex.Lock();

			while (_value <= 0) {
				_condition.Wait(&_mutex);
			}

			_value = _value - 1;

			_mutex.Unlock();
		}

		virtual void Notify() 
		{
			int count;

			_mutex.Lock();

			count = _value++;

			if (count > 0) {
				_condition.Notify();
			}

			_mutex.Unlock();
		}

};

CounterSemaphore s;
int n = 1000;
int nl = n/2;
int nu = n/2;

class TestClass : public jthread::Thread {

	private:
		bool lock;

	public:
		TestClass(bool lock) 
		{
			this->lock = lock;
		}

		virtual ~TestClass()
		{
		}

		virtual void Run()
		{
			if (lock == true) {
				s.Wait();

				std::cout << "Wait:: " << ++nl << std::endl;
			} else {
				s.Notify();

				std::cout << "Notify:: " << ++nu << std::endl;
			}
		}

};

int main() 
{
	int k = 0;
	TestClass *tests[n];

	do {
		int r = random()%2;

		if (r == 0) {
			if (nl > 0) {
				nl = nl - 1;

				tests[k++] = new TestClass(true);
			}
		} else {
			if (nu > 0) {
				nu = nu - 1;

				tests[k++] = new TestClass(false);
			}
		}
	} while (nl != 0 || nu != 0);

	for (int i=0; i<k; i++) {
		tests[i]->Start();
	}

	for (int i=0; i<k; i++) {
		tests[i]->WaitThread();
	}

	for (int i=0; i<k; i++) {
		delete tests[i];
	}

	return 0;
}

