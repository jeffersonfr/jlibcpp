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
#include "jshared/jnamedsemaphore.h"
#include "jexception/jsemaphoreexception.h"
#include "jexception/jsemaphoretimeoutexception.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

namespace jshared {

NamedSemaphore::NamedSemaphore(std::string name):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::NamedSemaphore");
	
	int fd;

	fd = open(name.c_str(), O_RDWR, 0666);

	if (fd < 0) {
		throw jexception::SemaphoreException("Cannot open the semaphore resource file");
	}

	_handler = (struct jnamedsemaphore_t *) mmap(nullptr, sizeof(struct jnamedsemaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close (fd);
}

NamedSemaphore::NamedSemaphore(std::string name, int access):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::NamedSemaphore");
	
	pthread_mutexattr_t psharedm;
	pthread_condattr_t psharedc;
	int fd;

	fd = open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, access);

	if (fd < 0) {
		throw jexception::SemaphoreException("Cannot create a semaphore resource file");
	}

	if (ftruncate(fd, sizeof(struct jnamedsemaphore_t)) != 0) {
		throw jexception::SemaphoreException("Semaphore resource file truncate error");
	}

	pthread_mutexattr_init(&psharedm);
	pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
	pthread_condattr_init(&psharedc);
	pthread_condattr_setpshared(&psharedc, PTHREAD_PROCESS_SHARED);

	_handler = (struct jnamedsemaphore_t *)mmap(nullptr, sizeof(struct jnamedsemaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close (fd);

	pthread_mutex_init(&_handler->lock, &psharedm);
	pthread_cond_init(&_handler->nonzero, &psharedc);

	_handler->count = 0;
}

NamedSemaphore::~NamedSemaphore()
{
	Release();
}

void NamedSemaphore::NamedSemaphore::Wait()
{
	pthread_mutex_lock(&_handler->lock);
	
	while (_handler->count == 0) {
		pthread_cond_wait(&_handler->nonzero, &_handler->lock);
	}

	_handler->size--;
	_handler->count--;
	
	pthread_mutex_unlock(&_handler->lock);
}

void NamedSemaphore::Wait(uint64_t time_)
{
	struct timespec t;
	int result = 0;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	pthread_mutex_lock(&_handler->lock);

	result = pthread_cond_timedwait(&_handler->nonzero, &_handler->lock, &t);

	pthread_mutex_unlock(&_handler->lock);

	if (result == ETIMEDOUT) {
		throw jexception::SemaphoreTimeoutException("Semaphore wait timeout");
	} else if (result < 0) {
		throw jexception::SemaphoreException("Semaphore wait failed");
	}
}

void NamedSemaphore::NamedSemaphore::Notify()
{
	pthread_mutex_lock(&_handler->lock);

	_handler->size++;

	if (_handler->count == 0) {
		if (pthread_cond_signal(&_handler->nonzero) != 0) {
			throw jexception::SemaphoreException("Condition notify error");
		}
	}

	_handler->count++;

	pthread_mutex_unlock(&_handler->lock);
}

void NamedSemaphore::NamedSemaphore::NotifyAll()
{
	pthread_mutex_lock(&_handler->lock);

	if (_handler->size < 0) {
		_handler->count = -_handler->size;
		_handler->size = 0;
	}

	if (pthread_cond_broadcast(&_handler->nonzero) != 0) {
		throw jexception::SemaphoreException("Condition notify all error");
	}

	pthread_mutex_unlock(&_handler->lock);
}

void NamedSemaphore::Release() 
{
	munmap((void *)_handler, sizeof(struct jnamedsemaphore_t));
}

}

