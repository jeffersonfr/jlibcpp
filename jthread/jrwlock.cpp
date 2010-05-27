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

RWLock::RWLock():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::RWLock");
	
#ifdef _WIN32
	InitializeSRWLock(&_rwlock);
#else 
	pthread_rwlock_init(&_rwlock, NULL);
#endif
}

RWLock::~RWLock()
{
#ifdef _WIN32
#else 
	pthread_rwlock_destroy(&_rwlock);
#endif
}

void RWLock::ReadLock()
{
#ifdef _WIN32
	AcquireSRWLockShared(&_rwlock);
#else 
	if (pthread_rwlock_rdlock(&_rwlock) != 0) {
		if (errno == EDEADLK) {
			throw MutexException("Error check monitor, dead lock ... !");
		}
	}
#endif
}

void RWLock::WriteLock()
{
#ifdef _WIN32
	AcquireSRWLockExclusive(&_rwlock);
#else 
	if (pthread_rwlock_wrlock(&_rwlock) != 0) {
		if (errno == EDEADLK) {
			throw MutexException("Error check monitor, dead lock ... !");
		}
	}
#endif
}

void RWLock::ReadUnlock()
{
#ifdef _WIN32
	ReleaseSRWLockShared(&_rwlock);
#else 
	if (pthread_rwlock_unlock(&_rwlock) != 0) {
		if (errno == EINVAL || errno == EFAULT || errno == EPERM) {
			throw MutexException("Error check monitor, calling thread does ... !");
		}
	}
#endif
}

void RWLock::WriteUnlock()
{
#ifdef _WIN32
	ReleaseSRWLockExclusive(&_rwlock);
#else 
	if (pthread_rwlock_unlock(&_rwlock) != 0) {
		if (errno == EINVAL || errno == EFAULT || errno == EPERM) {
			throw MutexException("Error check monitor, calling thread does ... !");
		}
	}
#endif
}

bool RWLock::TryReadLock()
{
#ifdef _WIN32
	if (TryAcquireSRWLockShared(&_rwlock) == 0) {
		return false;
	}
#else 
	if (pthread_rwlock_tryrdlock(&_rwlock) != 0) {
		return false;
	}
#endif 

	return true;
}

bool RWLock::TryWriteLock()
{
#ifdef _WIN32
	if (TryAcquireSRWLockExclusive(&_rwlock) == 0) {
		return false;
	}
#else 
	if (pthread_rwlock_trywrlock(&_rwlock) != 0) {
		return false;
	}
#endif 

	return true;
}

}

