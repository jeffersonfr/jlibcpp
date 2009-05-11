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
#include "jsemaphore.h"
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"
#include "jmutex.h"

#ifdef _WIN32
#else
#include <semaphore.h>
#include <errno.h>
#endif

namespace jthread {

bool is_wait_time_out = false;
	
Semaphore::Semaphore(int value_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Semaphore");
	
	_is_open = false;
	
#ifdef _WIN32
	_sa = NULL;
	_semaphore = NULL;
	_max_count = 65536; // CHANGE:: pode aumentar
	_inherit = TRUE;

	_sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
	_sa->nLength = sizeof(SECURITY_ATTRIBUTES);
    _sa->lpSecurityDescriptor = NULL;
    _sa->bInheritHandle = TRUE;

	if (_max_count <= 0) {
		throw SemaphoreException("Semaphre value can't be <= 0");
	}

	if (value_ < 0) {
		value_ = 0;
	}

	if (value_ > _max_count) {
		value_ = _max_count;
	}

	// if ((_semaphore = CreateSemaphore(_sa, _count, _max_count, "NTcourse.semaphore.empty")) == NULL) {
	if ((_semaphore = CreateSemaphore(_sa, value_, _max_count, NULL)) == NULL) {
		// if ((_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, _inherit, "NTcourse.semaphore.empty")) == NULL) {
			DWORD code = GetLastError();
			LPTSTR msg;

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				0,			        // no source buffer needed
				code,				// error code for this message
				0,					// default language ID
				(LPTSTR)&msg,		// allocated by fcn
				0,					// minimum size of buffer
				(va_list *)NULL);	// no inserts

			throw SemaphoreException(msg);
		// }
	}
#else
	if (sem_init(&_semaphore, 0, value_) < 0) {
		if (errno == EINVAL) {
	    	throw SemaphoreException("Operation would increase the semaphore count !");
		} else {
			throw SemaphoreException("Unknown semaphore error !");
		}
	}
#endif

	_is_open = true;
}

Semaphore::~Semaphore()
{
	if (_is_open == true) {
		_is_open = false;

#ifdef _WIN32
		if ((void *)_sa != NULL) {
			HeapFree(GetProcessHeap(), 0, _sa);
		}

		if ((void *)_semaphore != NULL) {
			if (CloseHandle(_semaphore) == 0) {
				throw SemaphoreException("Close semaphore failed");
			}

			_semaphore = NULL;
		}
#else
		while (sem_destroy(&_semaphore) < 0) {
			if (errno == EBUSY) {
				Notify();
			}
		}

		sem_destroy(&_semaphore);
#endif
	}
}

void Semaphore::Wait()
{
	if (_is_open == false) {
		return;
	}

#ifdef _WIN32
	switch (WaitForSingleObject(_semaphore, INFINITE)) {
		case WAIT_OBJECT_0:
			break;
		default:
			throw SemaphoreException("Waiting semaphore failed"); 
	}
#else
	if (sem_wait(&_semaphore) != 0) {
		throw SemaphoreException("Semaphore waiting failed");
	}
#endif
}

void Semaphore::Wait(long long time_)
{
	if (_is_open == false) {
		return;
	}
	
#ifdef _WIN32
	time_ /= 100000L;
	
	switch (WaitForSingleObject(_semaphore, (unsigned int)time_)) {
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			throw SemaphoreException("Waiting time out exception");
		default:
			throw SemaphoreException("Waiting semaphore failed"); 
	}
#else
	struct timespec t;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (long long)(time_/1000000LL);
	t.tv_nsec += (long long)((time_%1000000LL)*1000LL);

	while ((result = sem_timedwait(&_semaphore, &t)) == -1 && errno == EINTR) {
		continue; 
	}

	if (result == -1) {
		if (errno == ETIMEDOUT) {
			throw SemaphoreTimeoutException("Semaphore wait timeout");
		} else {
			throw SemaphoreTimeoutException("Semaphore wait failed");
		}
	}
#endif
}

void Semaphore::Notify()
{
	AutoLock lock(&mutex);

	if (_is_open == false) {
		return;
	}
	
#ifdef _WIN32
		if (ReleaseSemaphore(_semaphore, 1, NULL) == 0) {
			throw SemaphoreException("Notify semaphore failed");
		}
#else
	if (sem_post(&_semaphore) < 0) {
		if (errno == ERANGE) {
			throw SemaphoreException("Operation would increase the semaphore count !");
		} else {
			throw SemaphoreException("Notify semaphore failed");
		}
	}
#endif
}

void Semaphore::NotifyAll()
{
	AutoLock lock(&mutex);

	if (_is_open == false) {
		return;
	}
	
#ifdef _WIN32
	while (ReleaseSemaphore(_semaphore, 1, NULL) != 0);
#else
	while (GetValue() <= 0) {
		if (sem_post(&_semaphore) < 0) {
			throw SemaphoreException("Notify semaphore failed");
		}
	}
#endif
}

bool Semaphore::TryWait()
{
	AutoLock lock(&mutex);

	if (_is_open == false) {
		return false;
	}
	
#ifdef _WIN32
	switch (WaitForSingleObject(_semaphore, 0L)) {
		case WAIT_OBJECT_0:
			return true;
		default:
			return false; 
	}

	return true;
#else
	if (sem_trywait(&_semaphore) < 0) {
		if (errno == EAGAIN) {
			return false;
		}

		throw SemaphoreException("Unknown semaphore error !");
	}

	return true;
#endif
}

int Semaphore::GetValue()
{
	if (_is_open == false) {
		return 0;
	}

#ifdef _WIN32
	return 0;
#else
	int r;
    
	sem_getvalue(&_semaphore, &r);
    
	return r;
#endif
}

void Semaphore::Release()
{
}

}

