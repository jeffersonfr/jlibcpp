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
#include "jsemaphore.h"
#include "jautolock.h"
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"

namespace jthread {

Semaphore::Semaphore(int key, int value):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Semaphore");
	
	_counter = 0;

#ifdef _WIN32
	BOOL inherit = TRUE;

	_sa = NULL;
	_handler = NULL;

	_sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
	
	_sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	_sa->lpSecurityDescriptor = NULL;
	_sa->bInheritHandle = TRUE;

	if (value < 0) {
		value = 0;
	}

	// if ((_handler = CreateSemaphore(_sa, _count, _max_count, "NTcourse.semaphore.empty")) == NULL) {
	if ((_handler = CreateSemaphore(_sa, value, 65535, NULL)) == NULL) {
		// if ((_handler = OpenSemaphore(SEMAPHORE_ALL_ACCESS, inherit, "NTcourse.semaphore.empty")) == NULL) {
		DWORD code = GetLastError();
		char *msg = NULL;

		FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				0,			        	// no source buffer needed
				code,					// error code for this message
				0,						// default language ID
				(LPTSTR)msg,			// allocated by fcn
				0,						// minimum size of buffer
				(va_list *)NULL);		// no inserts

		throw SemaphoreException(msg);
		// }
	}
#else
	if (sem_init(&_handler, key, value) < 0) {
		if (errno == EINVAL) {
			throw SemaphoreException("Operation would increase the semaphore count !");
		} else {
			throw SemaphoreException("Unknown semaphore error !");
		}
	}
#endif
}

Semaphore::~Semaphore()
{
	Release();
}

void Semaphore::Wait()
{
#ifdef _WIN32
	switch (WaitForSingleObject(_handler, INFINITE)) {
		case WAIT_OBJECT_0:
			break;
		default:
			throw SemaphoreException("Waiting semaphore failed"); 
	}
#else
	{
		AutoLock lock(&_mutex);

		_counter = _counter + 1;
	}

	if (sem_wait(&_handler) != 0) {
		AutoLock lock(&_mutex);

		_counter = _counter - 1;

		throw SemaphoreException("Semaphore waiting failed");
	}
#endif
}

void Semaphore::Wait(uint64_t time_)
{
#ifdef _WIN32
	time_ /= 100000L;
	
	switch (WaitForSingleObject(_handler, (unsigned int)time_)) {
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			throw SemaphoreTimeoutException("Waiting time out exception");
		default:
			throw SemaphoreException("Waiting semaphore failed"); 
	}
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

	while ((result = sem_timedwait(&_handler, &t)) == -1 && errno == EINTR) {
		continue; 
	}

	AutoLock lock(&_mutex);

	if (result == -1) {
		_counter = _counter - 1;

		if (errno == ETIMEDOUT) {
			throw SemaphoreTimeoutException("Semaphore wait timeout");
		} else {
			throw SemaphoreException("Semaphore wait failed");
		}
	}
#endif
}

void Semaphore::Notify()
{
	AutoLock lock(&_mutex);

#ifdef _WIN32
		if (ReleaseSemaphore(_handler, 1, NULL) == 0) {
			throw SemaphoreException("Notify semaphore failed");
		}
#else
	if (sem_post(&_handler) < 0) {
		if (errno == ERANGE) {
			throw SemaphoreException("Operation would increase the semaphore count !");
		} else {
			throw SemaphoreException("Notify semaphore failed");
		}
	}
#endif

	_counter = _counter - 1;
}

void Semaphore::NotifyAll()
{
	AutoLock lock(&_mutex);

	int r = _counter;

#ifdef _WIN32
	LONG value;

	ReleaseSemaphore(_handler, 0, &value);

	if (value > 0) {
		ReleaseSemaphore(_handler, value, NULL);
	}
#else
	for (int i=0; i<r; i++) {
		sem_post(&_handler);
	}
#endif
	
	_counter = _counter - r;
}

bool Semaphore::TryWait()
{
#ifdef _WIN32
	switch (WaitForSingleObject(_handler, 0L)) {
		case WAIT_OBJECT_0:
			return true;
		default:
			return false; 
	}

	return true;
#else
	if (sem_trywait(&_handler) < 0) {
		if (errno != EAGAIN) {
			throw SemaphoreException("Unknown semaphore error !");
		}
	}

	return true;
#endif
}

int Semaphore::GetValue()
{
#ifdef _WIN32
	LONG value;

	ReleaseSemaphore(_handler, 0, &value);
	
	return value;
#else
	int r;
    
	sem_getvalue(&_handler, &r);
    
	return r;
#endif
}

void Semaphore::Release()
{
	try {
		NotifyAll();
	} catch (SemaphoreException &) {
	}

#ifdef _WIN32
	if ((void *)_sa != NULL) {
		HeapFree(GetProcessHeap(), 0, _sa);
	}

	if ((void *)_handler != NULL) {
		if (CloseHandle(_handler) == 0) {
			throw SemaphoreException("Close semaphore failed");
		}

		_handler = NULL;
	}
#else
	sem_destroy(&_handler);
#endif
}

}

