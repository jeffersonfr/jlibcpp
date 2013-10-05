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
#include "jbarrier.h"
#include "jsemaphoreexception.h"

#define BARRIER_FLAG (1UL << 31)

namespace jthread {

Barrier::Barrier(uint32_t locks):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Barrier");

#ifdef _WIN32
	_counter = locks;
	_threshold = locks;
	_cycle = 0;
	_is_valid = true;

	pthread_mutexattr_t attr;

	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
	pthread_mutexattr_setprioceiling(&attr, 0); 

	if (pthread_mutex_init(&_mutex, &attr) != 0) {
		throw SemaphoreException("Init barrier mutex error");
	}

	if (pthread_cond_init(&_condition, NULL) != 0) {
		pthread_mutex_destroy(&_mutex);

		throw SemaphoreException("Init barrier semaphore error");
	}
#else
	if (pthread_barrier_init(&_barrier, NULL, locks) != 0) {
		throw SemaphoreException("Init barrier error");
	}
#endif
}

Barrier::~Barrier()
{
	Release();
}

void Barrier::Release()
{
#ifdef _WIN32
	if (_is_valid == false) {
		// return EINVAL;
	}

	pthread_mutex_lock(&_mutex);

	if (_counter != _threshold) {
		pthread_mutex_unlock(&_mutex);

		throw SemaphoreException("Barrier is busy");
	}

	_is_valid = false;
	
	pthread_mutex_unlock(&_mutex);

	pthread_cond_destroy(&_condition);
	pthread_mutex_destroy(&_mutex);
#else
	pthread_barrier_destroy(&_barrier);
#endif
}

void Barrier::Wait()
{
#ifdef _WIN32
	int cancel, tmp, cycle;

	if (_is_valid == false) {
		throw SemaphoreException("Invalid barrier");
	}

	pthread_mutex_lock(&_mutex);

	// Remember which cycle we're on
	cycle = _cycle;

	if (--_counter == 0) {
		_cycle = !_cycle;
		_counter = _threshold;

		if (pthread_cond_broadcast(&_condition) != 0) {
			throw SemaphoreException("Barrier wait exception");
		}
		// The last thread into the barrier will return status -1 rather than 0, 
		// so that it can be used to perform some special serial code following the barrier
	} else {
		// Wait with cancellation disabled, because barrier_wait should not be a 
		// cancellation point
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);

		// Wait until the barrier's cycle changes, which means that it has been 
		// broadcast, and we don't want to wait anymore
		while (cycle == _cycle) {
			if (pthread_cond_wait(&_condition, &_mutex) != 0) {
				break;
			}
		}

		pthread_setcancelstate(cancel, &tmp);
	}

	// Ignore an error in unlocking. It shouldn't happen, and reporting it here 
	// would be misleading -- the barrier wait completed, after all, whereas 
	// returning, for example, EINVAL would imply the wait had failed. The next 
	// attempt to use the barrier *will* return an error, or hang, due to whatever 
	// happened to the mutex
	pthread_mutex_unlock(&_mutex);
#else
	// PTHREAD_BARRIER_SERIAL_THREAD: broadcast wait() unlock
	// 0: otherwise
	if (pthread_barrier_wait(&_barrier) == EINVAL) {
		throw SemaphoreException("Barrier wait error");
	}
#endif
}

}

