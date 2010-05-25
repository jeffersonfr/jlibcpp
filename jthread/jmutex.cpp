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
#include "jthreadlib.h"

namespace jthread {

Mutex::Mutex(jmutex_type_t type_, jmutex_protocol_t protocol_, bool block_in_death):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Mutex");
	
#ifdef _WIN32
	_mutex = CreateMutex(0, FALSE, 0);
#elif _CYGWIN 
	_mutex = CreateMutex(0, FALSE, 0);
#else 
	pthread_mutexattr_t attr;
    
	pthread_mutexattr_init(&attr);
    
	if (type_ == FAST_MUTEX) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);
	} else if (type_ == RECURSIVE_MUTEX) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	} else if (type_ == ERROR_CHECK_MUTEX) {
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
	_lock_count = 0;
}

Mutex::~Mutex()
{
#ifdef _WIN32
	if(WaitForSingleObject(_mutex, (DWORD)0) == WAIT_TIMEOUT ) {
		// return RC_NOT_OWNER;
	}

	CloseHandle(_mutex);
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
	return (_lock_count != 0);
}

void Mutex::Lock()
{
#ifdef _WIN32
	if (WaitForSingleObject(_mutex, INFINITE) == WAIT_FAILED) {
		throw MutexException("Mutex lock failed !");
	}
#else 
	if (pthread_mutex_lock(&_mutex)) {
		if (errno == EDEADLK) {
			throw MutexException("Error check monitor, dead lock ... !");
		} else {
			throw MutexException("Mutex lock failed !");
		}
	}
#endif

	_lock_count++;
}

void Mutex::Unlock()
{
#ifdef _WIN32
	if (ReleaseMutex(_mutex) == 0) {
		throw MutexException("Mutex unlock failed !");
	}
#else 
	if (pthread_mutex_unlock(&_mutex) != 0) {
		if (errno == EINVAL || errno == EFAULT || errno == EPERM) {
			throw MutexException("Error check monitor, calling thread does ... !");
		} else {
			// throw MutexException("Mutex unlock failed !");
		}
	}
#endif

	if (_type == FAST_MUTEX) {
		_lock_count = 0;
	} else {
		_lock_count--;
	}
}

bool Mutex::TryLock()
{
#ifdef _WIN32
	if (WaitForSingleObject(_mutex, 1000) == WAIT_FAILED) { // wait one second
		return false;
	}
#else 
	if (pthread_mutex_trylock(&_mutex) != 0) {
		if (errno == EBUSY) {
			return false;
		} else {
			// throw
			return false;
		}
	}
#endif 
    
	_lock_count++;

	return true;
}

std::string Mutex::what()
{
	std::ostringstream o;

	o << "Mutex" << "@" << (unsigned long)this;
	
	return o.str();
}

}
