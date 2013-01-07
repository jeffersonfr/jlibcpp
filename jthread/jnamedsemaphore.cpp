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
#include "jnamedsemaphore.h"
#include "jautolock.h"
#include "jsemaphoreexception.h"
#include "joutofmemoryexception.h"
#include "jsemaphoretimeoutexception.h"

namespace jthread {

NamedSemaphore::NamedSemaphore(std::string file, int mode, int value):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::NamedSemaphore");
	
	_counter = 0;

#ifdef _WIN32
#else
	_handler = sem_open(file.c_str(), O_CREAT | O_EXCL | O_RDWR, mode, value);

	if (_handler == (void *)-1 || _handler == SEM_FAILED) {
		_handler = NULL;

		if (errno == EACCES) {
			throw SemaphoreException("Access denied to the resource");
		} else if (errno == EEXIST) {
			throw SemaphoreException("Resource already exists");
		} else if (errno == EINVAL) {
			throw SemaphoreException("Increase of value not allowed");
		} else if (errno == EMFILE) {
			throw SemaphoreException("Maximum number of resources has been reached");
		} else if (errno == ENAMETOOLONG) {
			throw SemaphoreException("Name was too long");
		} else if (errno == ENFILE) {
			throw SemaphoreException("Number of open files has been reached");
		} else if (errno == ENOMEM) {
			throw jcommon::OutOfMemoryException("Insufficient memory");
		} else {
			throw jcommon::RuntimeException("Unknown resource exception");
		}
	}
#endif
}

NamedSemaphore::NamedSemaphore(std::string file):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::NamedSemaphore");
	
	_counter = 0;

#ifdef _WIN32
#else
	_handler = sem_open(file.c_str(), O_RDWR);
	
	if (_handler == (void *)-1 || _handler == SEM_FAILED) {
		_handler = NULL;

		if (errno == EACCES) {
			throw SemaphoreException("Access denied to the resource");
		} else if (errno == EEXIST) {
			throw SemaphoreException("Resource already exists");
		} else if (errno == EINVAL) {
			throw SemaphoreException("Increase of value not allowed");
		} else if (errno == EMFILE) {
			throw SemaphoreException("Maximum number of resources has been reached");
		} else if (errno == ENAMETOOLONG) {
			throw SemaphoreException("Name was too long");
		} else if (errno == ENFILE) {
			throw SemaphoreException("Number of open files has been reached");
		} else if (errno == ENOENT) {
			throw SemaphoreException("Resource does not exists");
		} else if (errno == ENOMEM) {
			throw jcommon::OutOfMemoryException("Insufficient memory");
		} else {
			throw jcommon::RuntimeException("Unknown resource exception");
		}
	}
#endif
}

NamedSemaphore::~NamedSemaphore()
{
	Release();
}

void NamedSemaphore::Wait()
{
#ifdef _WIN32
#else
	{
		AutoLock lock(&_mutex);

		_counter = _counter + 1;
	}

	if (sem_wait(_handler) != 0) {
		AutoLock lock(&_mutex);

		_counter = _counter - 1;

		throw SemaphoreException("NamedSemaphore waiting failed");
	}
#endif
}

void NamedSemaphore::Wait(uint64_t time_)
{
#ifdef _WIN32
#else
	struct timespec t;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	{
		AutoLock lock(&_mutex);

		_counter = _counter + 1;
	}

	while ((result = sem_timedwait(_handler, &t)) == -1 && errno == EINTR) {
		continue; 
	}

	AutoLock lock(&_mutex);

	if (result == -1) {
		_counter = _counter - 1;

		if (errno == ETIMEDOUT) {
			throw SemaphoreTimeoutException("NamedSemaphore wait timeout");
		} else {
			throw SemaphoreException("NamedSemaphore wait failed");
		}
	}
#endif
}

void NamedSemaphore::Notify()
{
	AutoLock lock(&_mutex);

#ifdef _WIN32
#else
	if (sem_post(_handler) < 0) {
		if (errno == ERANGE) {
			throw SemaphoreException("Operation would increase the semaphore count");
		} else {
			throw SemaphoreException("Notify semaphore failed");
		}
	}
#endif

	_counter = _counter - 1;
}

void NamedSemaphore::NotifyAll()
{
	AutoLock lock(&_mutex);

	int r = _counter;

#ifdef _WIN32
#else
	for (int i=0; i<r; i++) {
		sem_post(_handler);
	}
#endif
	
	_counter = _counter - r;
}

bool NamedSemaphore::TryWait()
{
#ifdef _WIN32
	return false;
#else
	if (sem_trywait(_handler) < 0) {
		if (errno != EAGAIN) {
			throw SemaphoreException("Unknown semaphore error");
		}

		return false;
	}

	return true;
#endif
}

int NamedSemaphore::GetValue()
{
#ifdef _WIN32
#else
	int r;
    
	sem_getvalue(_handler, &r);
    
	return r;
#endif
}

void NamedSemaphore::Release()
{
	try {
		// NotifyAll();
	} catch (SemaphoreException &) {
	}

#ifdef _WIN32
#else
	sem_close(_handler);

	_handler = NULL;
#endif
}

}

