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
#include "jcondition.h"
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"
#include "jautolock.h"

#include <errno.h>

// #define SINGLE_WAIT_CONDITION

namespace jthread {

Condition::Condition(int nblock):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Condition");

#ifdef _WIN32
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
	} catch (SemaphoreException &e) {
	}
		
#ifdef _WIN32
	_semaphore.Release();
#else
	pthread_cond_destroy(&_condition);
#endif
}

void Condition::Wait(Mutex *mutex)
{
#ifdef _WIN32
	_semaphore.Wait();
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

void Condition::Wait(long long time_, Mutex *mutex)
{
#ifdef _WIN32
	_semaphore.Wait(time_);
#else
	struct timespec t;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (long long)(time_/1000000LL);
	t.tv_nsec += (long long)((time_%1000000LL)*1000LL);

	if ((void *)mutex == NULL) {
		jthread::AutoLock lock(&_monitor);

		result = pthread_cond_timedwait(&_condition, &_monitor._mutex, &t);
	} else {
		result = pthread_cond_timedwait(&_condition, &mutex->_mutex, &t);
	}

	if (result == ETIMEDOUT) {
		throw SemaphoreTimeoutException("Semaphore wait timeout");
	} else if (result < 0) {
		throw SemaphoreTimeoutException("Semaphore wait failed");
	}
#endif
}

void Condition::Notify()
{
#ifdef _WIN32
	_semaphore.Notify();
#else
	if (pthread_cond_signal(&_condition) != 0) {
		throw SemaphoreException("Condition notify error !");
	}
#endif
}

void Condition::NotifyAll()
{
#ifdef _WIN32
	_semaphore.NotifyAll();
#else
	if (pthread_cond_broadcast(&_condition) != 0) {
		throw SemaphoreException("Condition notify all error !");
	}
#endif
}

}

