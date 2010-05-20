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
#include "jevent.h"
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"

#include <errno.h>

namespace jthread {

Event::Event()
{
#ifdef _WIN32
	// single event
	// event = CreateEvent(NULL, FALSE, FALSE, NULL);
	// broadcast event
	_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (_event == INVALID_HANDLE_VALUE) {
		throw SemaphoreException("Init semaphore error !");
	}
#else
	_signaled = false;
	_count = 0;

	if (pthread_cond_init(&_condition, NULL) != 0) {
		throw SemaphoreException("Init semaphore error !");
	}
#endif
}

Event::~Event()
{
	Release();
}

void Event::Reset() 
{
#ifdef _WIN32
	if (!ResetEvent(_event)) {
		// throw ?
	}
#else
	_mutex.Lock();
	_signaled = false;
	_mutex.Unlock();
#endif
}

void Event::Wait()
{
#ifdef _WIN32
	if (WaitForSingleObject(_event, INFINITE) == WAIT_FAILED) {
		throw SemaphoreException("Condition wait error !");
	}
#else
	int rc = 0;
	
	_mutex.Lock();
	
	if (_signaled) {
		_signaled = false;
	} else {
		_count++;
		rc = pthread_cond_wait(&_condition, &_mutex._mutex);
		_count--;
	}

	_mutex.Unlock();
	
	if (rc != 0) {
		throw SemaphoreException("Condition wait error !");
	}
#endif
}

void Event::Wait(uint64_t time_)
{
#ifdef _WIN32
	DWORD millisecs,
				status;

	millisecs = (DWORD)(time_*1000LL);
	status = WaitForSingleObject(_event, millisecs);

	if (status == WAIT_TIMEOUT) {
		throw SemaphoreTimeoutException("Semaphore wait timeout");
	} else if (status == WAIT_FAILED) {
		throw SemaphoreTimeoutException("Semaphore wait failed");
	}
#else
	struct timespec t;
	int rc = 0;

	_mutex.Lock();
	
	if (_signaled) {
		_signaled = false;
	} else {
		clock_gettime(CLOCK_REALTIME, &t);
		
		t.tv_sec += (long long)(time_/1000000LL);
		t.tv_nsec += (long long)((time_%1000000LL)*1000LL);
		
		_count++;
		rc = pthread_cond_timedwait(&_condition, &_mutex._mutex, &t);
		_count--;
	}

	_mutex.Unlock();
	
	if (rc != 0) {
		if (rc == ETIMEDOUT) {
			throw SemaphoreTimeoutException("Semaphore wait timeout");
		} else {
			throw SemaphoreTimeoutException("Semaphore wait failed");
		}
	}
#endif
}

void Event::Notify()
{
#ifdef _WIN32
	BOOL success = FALSE;

	// single event
	success = SetEvent(_event);

	if (!success) {
		throw SemaphoreException("Semaphore notify error !");
	}
#else
	int rc = 0;

	_mutex.Lock();
	
	if (_count == 0) {
		_signaled = true;
	} else {
		rc = pthread_cond_signal(&_condition); 
	}	

	_mutex.Unlock();

	if (rc != 0) {
		throw SemaphoreException("Semaphore notify error !");
	}
#endif
}

void Event::NotifyAll()
{
#ifdef _WIN32
	BOOL success = FALSE;

	// broadcast event
	success = PulseEvent(_event);

	if (!success) {
		throw SemaphoreException("Semaphore notify error !");
	}
#else
	int rc = 0;

	_mutex.Lock();
	
	rc = pthread_cond_broadcast(&_condition); 

	_signaled = false;
	_count = 0;
	
	_mutex.Unlock();

	if (rc != 0) {
		throw SemaphoreException("Semaphore notify all error !");
	}
#endif
}

void Event::Release()
{
#ifdef _WIN32
	if (!CloseHandle(_event)) {
		// throw ?
	}

	_event = INVALID_HANDLE_VALUE;
#else
	while (pthread_cond_destroy(&_condition) == EBUSY) {
		Notify();
	}
#endif
}

}
