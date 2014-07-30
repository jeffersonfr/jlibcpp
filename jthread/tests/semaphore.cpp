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
#include "jsemaphoretimeoutexception.h"

#include <iostream>
#include <vector>

#include <unistd.h>

#define INTERACTIONS	1000

class LocalSemaphore {

	private:
		jthread::Mutex _mutex;
		jthread::Mutex _value;
		jthread::Condition _condition;
		int _counter;

	public:
		LocalSemaphore(int value = 0)
		{
			_counter = value;

			if (_counter < 0) {
				_counter = 0;
			}
		}

		virtual ~LocalSemaphore()
		{
		}

		void Wait()
		{
			_mutex.Lock();

			_value.Lock();
			_counter = _counter - 1;
			_value.Unlock();

			if (_counter < 0) {
				_condition.Wait(&_mutex);
			}

			_mutex.Unlock();
		}

		void Wait(int64_t time)
		{
			_mutex.Lock();

			_value.Lock();
			_counter = _counter - 1;
			_value.Unlock();

			if (_counter < 0) {
				if (time <= 0LL) {
					_condition.Wait(&_mutex);
				} else {
					try {
						_condition.Wait(time, &_mutex);
					} catch (jthread::SemaphoreTimeoutException &e) {
						_value.Lock();
						_counter = _counter + 1;
						_value.Unlock();
					}
				}
			}

			_mutex.Unlock();
		}

		void Notify()
		{
			_value.Lock();
			_counter = _counter + 1;
			_value.Unlock();

			if (_counter >= 0) {
				_condition.Notify();
			} else {
				_mutex.Unlock();
			}
		}

		void NotifyAll()
		{
			_value.Lock();
			_counter = 0;
			_value.Unlock();

			_condition.NotifyAll();
		}

		int GetValue()
		{
			int value;

			_value.Lock();
			value = _counter;
			_value.Unlock();

			return value;
		}

};

class Test : public jthread::Thread {

	private:
		LocalSemaphore *_sem;

	public:
		Test(LocalSemaphore *sem)
		{
			_sem = sem;
		}
		
		virtual ~Test()
		{
			// std::cout << "Test Destructor" << std::endl;
		}

		virtual void Run() 
		{
			// std::cout << "Test running ..." << std::endl;

			_sem->Wait();
		}

};

int main() 
{
	LocalSemaphore s;

	for (int i=0; i<INTERACTIONS; i++) {
		s.Notify();

		std::cout << "+" << std::flush;
		// std::cout << "LocalSemaphore Notify [" << (i+1) << "]" << std::endl;
	}

	std::cout << std::endl;

	for (int i=0; i<INTERACTIONS; i++) {
		s.Wait();

		std::cout << "-" << std::flush;
		// std::cout << "LocalSemaphore Wait [" << (i+1) << "]" << std::endl;
	}

	std::cout << std::endl;

	std::vector<Test *> semaphores;

	for (int i=0; i<INTERACTIONS; i++) {
		Test *test = new Test(&s);

		test->Start();

		semaphores.push_back(test);
	}

	sleep(1);
	std::cout << "Semaphore Counter:: " << s.GetValue() << std::endl;

	s.NotifyAll();

	for (int i=0; i<INTERACTIONS; i++) {
		Test *test = *semaphores.begin();

		test->WaitThread();
	}

	for (int i=0; i<INTERACTIONS; i++) {
		Test *test = *semaphores.begin();

		semaphores.erase(semaphores.begin());

		delete test;
	}

	std::cout << "Semaphore Timed Wait (10 secs)" << std::endl;

	s.Wait(10000000LL);

	return 0;
}

