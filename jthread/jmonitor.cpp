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
#include "Stdafx.h"
#include "jmonitor.h"
#include "jthreadexception.h"
#include "jsemaphoretimeoutexception.h"

namespace jthread {

Monitor::Monitor()
{
	_holder = 0;
	_lock_count = 0;
}

Monitor::~Monitor()
{
	_mutex.Lock();

	if (readyQ.empty() == false || waitQ.empty() == false || _holder != NULL) {
		throw ThreadException("Monitor busy exception");
	}
	
	while (threadQ.empty() == false) {
		jmonitor_map_t *t = threadQ.begin()->second;

		threadQ.erase(threadQ.begin());

		delete t;
	}
	
	_mutex.Unlock();
}

jmonitor_map_t * Monitor::GetCurrentThread()
{
#ifdef _WIN32
	std::map<HANDLE, jmonitor_map_t *>::iterator i;
	HANDLE key = ::GetCurrentThread();
#else
	std::map<pthread_t, jmonitor_map_t *>::iterator i;
	pthread_t key = pthread_self();
#endif

	i = threadQ.find(key);

	if (i == threadQ.end()) {
		jmonitor_map_t *t = new jmonitor_map_t;

		t->signalled = false;

		threadQ[key] = t;

		return t;
	}

	return i->second;
}

void Monitor::Enter()
{
	_mutex.Lock();

	jmonitor_map_t *thread = GetCurrentThread();

	// printf("Monitor.lock: Requested lock on Monitor by thread::\n");

	if (_holder == 0) {
		// printf("Monitor.lock: Monitor lock granted immediately to thread as there are no waiters\n");

		// If no one is holding the lock, then we can grant it immediately
		_holder = thread;
		_lock_count = 1;
	} else if (_holder == thread) {
		// printf("Monitor.lock: Monitor lock incremented by thread\n");

		// If the lock is already held by the thread, then we simply increment the count
		_lock_count++;
	} else {
		// printf("Monitor.lock: Monitor locked by, therefore thread must wait\n");

		// We need to wait until the lock is released and can be granted to us. there may be 
		// other threads ahead of us who will get the lock before we do.
		thread->signalled = false;
		readyQ.push_back(thread);
		
		_mutex.Unlock();
		thread->sem.Wait();
		_mutex.Lock();

		// printf("Monitor.lock: Monitor lock granted to thread\n");
	}

	_mutex.Unlock();
}

bool Monitor::TryEnter()
{
	_mutex.Lock();
      
	jmonitor_map_t *thread = GetCurrentThread();

	// printf("Monitor.tryLock: Requested lock on Monitor by thread\n");

	if (_holder == 0) {
		// printf("Monitor.tryLock: Monitor lock granted to thread as there are no waiters\n");

		// If no one is holding the lock, then we can grant it immediately
		_holder = thread;
		_lock_count = 1;
	} else if (_holder == thread) {
		// printf("Monitor.tryLock: Monitor lock incremented by thread\n");

		// If the lock is already held by the thread, then we simply increment the count
		_lock_count++;
	} else {
		// printf("Monitor.tryLock: Monitor currently locked by, thread cannot be granted lock\n");

		// We need to wait until the lock is released and can be granted to us. There may be 
		// other threads ahead of us who will get the lock before we do.
		return false;
	}

	_mutex.Unlock();

	return true;
}

void Monitor::Exit()
{
	_mutex.Lock();

	jmonitor_map_t *thread = GetCurrentThread();

	if (_holder != thread) {
		_mutex.Unlock();

		if (_holder == 0) {
			throw ThreadException("Monitor unlock error: lock is not held");
		} else {
			throw ThreadException("Monitor unlock error: lock is held by other thread");
		}
	}

	_lock_count--;

	if (_lock_count == 0) {
		// printf("Monitor.unlock: Monitor unlocked by thread\n");

		// We grant the lock to the next waiting thread.
		_holder = 0;
		GrantMonitor();
	} else {
		// printf("Monitor.unlock: Monitor lock decremented by thread\n");
	}

	_mutex.Unlock();
}

void Monitor::GrantMonitor()
{
	if (_holder != NULL) {
		throw ThreadException("Grant monitor exception");
	}

	if (readyQ.empty() == false) {
		_holder = *readyQ.begin();
	}

	if (_holder != NULL) {
		// printf("Monitor.grantMonitor: Monitor granted to thread\n");

		_lock_count = 1;

		std::vector<jmonitor_map_t *>::iterator i = std::find(readyQ.begin(), readyQ.end(), _holder);

		if (i != readyQ.end()) {
			readyQ.erase(i);
		}

		_holder->sem.Notify();
	}
}

void Monitor::Wait()
{
	_mutex.Lock();

	jmonitor_map_t *thread = GetCurrentThread();

	if (_holder != thread) {
		_mutex.Unlock();

		if (_holder == 0) {
			throw ThreadException("Monitor wait error: lock is not held");
		} else {
			throw ThreadException("Monitor wait error: lock is held by other thread");
		}
	}

	// printf("Monitor.wait: Thread about to wait on Monitor\n");

	int savedCount = _lock_count;

	thread->signalled = false;
	waitQ.push_back(thread);
	_holder = 0;
	_lock_count = 0;
	GrantMonitor();
	_mutex.Unlock();
	thread->sem.Wait();
	_mutex.Lock();
	_lock_count = savedCount;
	// printf("Monitor.wait: Thread finished wait on Monitor\n");
	_mutex.Unlock();
}

bool Monitor::Wait(uint64_t timeout)
{
	_mutex.Lock();

	jmonitor_map_t *thread = GetCurrentThread();
	
	if (_holder != thread) {
		_mutex.Unlock();

		if (_holder == 0) {
			// printf("Monitor.wait: Error: wait requested by, but lock is not held\n");
			throw ThreadException("Monitor wait error: lock is not held");
		} else {
			// printf("Monitor.wait: Error: wait requested by, but lock is held by\n");
			throw ThreadException("Monitor wait error: lock is held by other thread");
		}
	}

	// printf("Monitor.wait: Thread about to wait on Monitor\n");
	int savedCount = _lock_count;

	thread->signalled = false;
	waitQ.push_back(thread);
	_holder = 0;
	_lock_count = 0;
	GrantMonitor();
	
	_mutex.Unlock();
	
	try {
		thread->sem.Wait(timeout);
	} catch (SemaphoreTimeoutException &) {
		std::vector<jmonitor_map_t *>::iterator i = std::find(waitQ.begin(), waitQ.end(), thread);

		if (i != waitQ.end()) {
			waitQ.erase(i);
		}
	}
	
	_mutex.Lock();

	if (_holder != thread) {
		if (thread->signalled != 0) {
			throw ThreadException("Thread was signalled");
		}

		if (_holder == 0) {
			_holder = thread;
		} else {
			readyQ.push_back(thread);
			
			_mutex.Unlock();
			thread->sem.Wait();
			_mutex.Lock();
			
			if (_holder != thread) {
				throw ThreadException("Holder thread cannot be requested");
			}
		}
	} else {
		_lock_count = savedCount;
		// printf("Monitor.wait: Thread finished timed wait on Monitor - result = \n");
		_mutex.Unlock();
		
		return true;
	}

	_lock_count = savedCount;
	// printf("Monitor.wait: Thread finished timed wait on Monitor - result = \n");
	_mutex.Unlock();

	return false;
}

void Monitor::Notify()
{
	_mutex.Lock();

	jmonitor_map_t *thread = NULL;
	
	if (waitQ.empty() == false) {
		thread = *waitQ.begin();
	}

	if (thread != 0) {
		// printf("Monitor.notify: Thread signaled on Monitor\n");
		thread->signalled = true;

		std::vector<jmonitor_map_t *>::iterator i = std::find(waitQ.begin(), waitQ.end(), thread);

		if (i != waitQ.end()) {
			waitQ.erase(i);
		}

		readyQ.push_back(thread);
		
		if (_holder == 0) {
			GrantMonitor();
		}
	}

	_mutex.Unlock();
}

void Monitor::NotifyAll()
{
	_mutex.Lock();

	jmonitor_map_t *thread = NULL;
	
	if (waitQ.empty() == false) {
		thread = *waitQ.begin();
	}

	while (thread != NULL) {
		// printf("Monitor.notifyAll: Thread signaled on Monitor\n");
		thread->signalled = true;

		std::vector<jmonitor_map_t *>::iterator i = std::find(waitQ.begin(), waitQ.end(), thread);

		if (i != waitQ.end()) {
			waitQ.erase(i);
		}

		readyQ.push_back(thread);

		thread = NULL;

		if (waitQ.empty() == false) {
			thread = *waitQ.begin();
		}
	}

	if (readyQ.empty() == false && _holder == NULL) {
		GrantMonitor();
	}

	_mutex.Unlock();
}

}	

