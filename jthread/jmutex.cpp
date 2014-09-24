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
#include "jmutex.h"
#include "jmutexexception.h"
#include "jtimeoutexception.h"

namespace jthread {

Mutex::Mutex(jmutex_type_t type_, jmutex_protocol_t protocol_, bool block_in_death):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Mutex");
	
#ifdef _WIN32
	InitializeCriticalSection(&_mutex);
	
	_lock_count = 0;
#else 
	pthread_mutexattr_t attr;
    
	pthread_mutexattr_init(&attr);
    
	if (type_ == JMT_FAST) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);
	} else if (type_ == JMT_RECURSIVE) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	} else if (type_ == JMT_ERROR_CHECK) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
	}

	/*
	if (protocol_ == NONE_PROT_MUTEX) {
		pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_NONE);
	} else if (protocol_ == INHERIT_PROT_MUTEX) {
		pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
	} else if (protocol_ == PROTECT_PROT_MUTEX) {
		pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_PROTECT);
	}

	if (block_in_death == true) {
		pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_STALLED_NP);
	} else {
		pthread_mutexattr_setrobust_np(&attr, PTHREAD_MUTEX_ROBUST_NP);
	}

	pthread_mutexattr_setprioceiling(&attr, 0); 
	*/

	pthread_mutex_init(&_mutex, &attr);
#endif

	_type = type_;
}

Mutex::~Mutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&_mutex);
#else 
	/*
	while (pthread_mutex_destroy(&_mutex) == EBUSY) {
		Lock();
		Unlock();
	}
	*/

	pthread_mutex_destroy(&_mutex);
#endif
}

bool Mutex::IsLocked()
{
	if (TryLock() == true) {
		Unlock();

		return true;
	}

	return false;
}

void Mutex::Lock()
{
#ifdef _WIN32
	EnterCriticalSection(&_mutex);
	
	_lock_count++;
#else 
	int r;

	if ((r = pthread_mutex_lock(&_mutex)) != 0) {
		if (r == EDEADLK) {
			throw MutexException("Error check monitor, dead lock");
		} else {
			throw MutexException("Mutex lock failed");
		}
	}
#endif
}

void Mutex::Lock(int time_)
{
#ifdef _WIN32
	EnterCriticalSection(&_mutex);

	_lock_count++;
#else 
	struct timespec t;

	clock_gettime(CLOCK_REALTIME, &t);

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	int r;

	if ((r = pthread_mutex_timedlock(&_mutex, &t)) != 0) {
		if (r == EDEADLK) {
			throw MutexException("Error check monitor, dead lock");
		} else if (r == ETIMEDOUT) {
			throw jcommon::TimeoutException("Mutex lock timeout");
		} else {
			throw MutexException("Mutex lock failed");
		}
	}
#endif
}

void Mutex::Unlock()
{
#ifdef _WIN32
	if (_type == JMT_FAST) {
		_lock_count = 0;
	} else {
		_lock_count--;
	}

	LeaveCriticalSection(&_mutex);
#else 
	int r;

	if ((r = pthread_mutex_unlock(&_mutex)) != 0) {
		if (r == EPERM) {
			// throw MutexException("Current thread does not own the mutex");
		} else {
			throw MutexException("Mutex unlock failed");
		}
	}
#endif
}

bool Mutex::TryLock()
{
#ifdef _WIN32
	if (TryEnterCriticalSection(&_mutex) == 0) {
		_lock_count++;

		return false;
	}
#else 
	int r;

	if ((r = pthread_mutex_trylock(&_mutex)) != 0) {
		if (r == EBUSY) {
			return false;
		} else if (r == EDEADLK) {
			return false;

			// throw MutexException("Dead lock in try lock");
		} else {
			throw MutexException("Mutex trylock failed");
		}
	}
#endif 

	return true;
}

}
