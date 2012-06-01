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
#include "jcondition.h"
#include "jsemaphoreexception.h"
#include "jautolock.h"
#include "jsemaphoretimeoutexception.h"

namespace jthread {

Condition::Condition(int nblock):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Condition");

#ifdef _WIN32
	InitializeConditionVariable(&_condition);
#else
	if (pthread_cond_init(&_condition, NULL) != 0) { //&cond) != 0) {
		throw SemaphoreException("Init condition error !");
	}
#endif
}

Condition::~Condition()
{
	Release();
}

void Condition::Release()
{
	try {
		NotifyAll();
	} catch (SemaphoreException &) {
	}
		
#ifdef _WIN32
	// _condition = INVALID_HANDLE_VALUE;
#else
	pthread_cond_destroy(&_condition);
#endif
}

void Condition::Wait(Mutex *mutex)
{
#ifdef _WIN32
	if ((void *)mutex == NULL) {
		jthread::AutoLock lock(&_monitor);

		if (SleepConditionVariableCS(&_condition, &_monitor._mutex, INFINITE) == 0) {
			throw SemaphoreException("Condition wait error !");
		}
	} else {
		if (SleepConditionVariableCS(&_condition, &mutex->_mutex, INFINITE) == 0) {
			throw SemaphoreException("Condition wait error !");
		}
	}
#else
	if ((void *)mutex == NULL) {
		jthread::AutoLock lock(&_monitor);

		if (pthread_cond_wait(&_condition, &_monitor._mutex) != 0) {
			throw SemaphoreException("Condition wait error !");
		}
	} else {
		if (pthread_cond_wait(&_condition, &mutex->_mutex) != 0) {
			throw SemaphoreException("Condition wait error !");
		}
	}
#endif
}

void Condition::Wait(uint64_t time_, Mutex *mutex)
{
#ifdef _WIN32
	DWORD d = (DWORD)(time_/1000LL);
	DWORD result;

	if ((void *)mutex == NULL) {
		jthread::AutoLock lock(&_monitor);

		result = SleepConditionVariableCS(&_condition, &_monitor._mutex, d);
	} else {
		result = SleepConditionVariableCS(&_condition, &mutex->_mutex, d);
	}

	if (result == 0) {
		if (GetLastError() == WAIT_TIMEOUT) {
			throw SemaphoreTimeoutException("Semaphore wait timeout");
		} else {
			throw SemaphoreException("Semaphore wait failed");
		}
	}
#else
	struct timespec t;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	if ((void *)mutex == NULL) {
		jthread::AutoLock lock(&_monitor);

		result = pthread_cond_timedwait(&_condition, &_monitor._mutex, &t);
	} else {
		result = pthread_cond_timedwait(&_condition, &mutex->_mutex, &t);
	}

	if (result == ETIMEDOUT) {
		throw SemaphoreTimeoutException("Semaphore wait timeout");
	} else if (result < 0) {
		throw SemaphoreException("Semaphore wait failed");
	}
#endif
}

void Condition::Notify()
{
#ifdef _WIN32
	WakeConditionVariable(&_condition);
#else
	if (pthread_cond_signal(&_condition) != 0) {
		throw SemaphoreException("Condition notify error !");
	}
#endif
}

void Condition::NotifyAll()
{
#ifdef _WIN32
	WakeAllConditionVariable(&_condition);
#else
	if (pthread_cond_broadcast(&_condition) != 0) {
		throw SemaphoreException("Condition notify all error !");
	}
#endif
}

}

