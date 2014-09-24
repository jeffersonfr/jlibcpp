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
#include "jfifomutex.h"
#include "jmutexexception.h"
#include "jtimeoutexception.h"

namespace jthread {

FifoMutex::FifoMutex():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::FifoMutex");
	
	_fifo_mutex.unused = NULL;
	_fifo_mutex.list = NULL;
	_fifo_mutex.active = NULL;	
	_fifo_mutex.owner = (pthread_t)-1;
}

FifoMutex::~FifoMutex()
{
	struct jfifo_mutex_list_t *curr, *next;

	_fifo_mutex.control.Lock();

	// Clear out the unused list.
	if (_fifo_mutex.unused) {
		next = _fifo_mutex.unused;
		_fifo_mutex.unused = NULL;

		while (next) {
			curr = next;
			next = next->next;
			curr->next = NULL;

			delete curr;
		}
	}

	// If the FIFO mutex is still in use, return EBUSY.
	if (_fifo_mutex.active) {
		_fifo_mutex.control.Unlock();
	}

	if (_fifo_mutex.list) {
		// If the first mutex is not immediately lockable, then the mutex is in use -- return EBUSY.
		if (_fifo_mutex.list->lock.TryLock() == false) {
			_fifo_mutex.control.Unlock();
		}

		_fifo_mutex.list->lock.Unlock();

		// Clean up the list.
		next = _fifo_mutex.list;
		_fifo_mutex.list = NULL;

		while (next) {
			curr = next;
			next = next->next;
			curr->next = NULL;

			delete curr;
		}
	}

	_fifo_mutex.control.Unlock();
}

bool FifoMutex::IsLocked()
{
	if (TryLock() == true) {
		Unlock();

		return true;
	}

	return false;
}

void FifoMutex::Lock()
{
	if (TryLock() == false) {
		throw MutexException("Fifo mutex lock exception");
	}
}

void FifoMutex::Lock(int time_)
{
	throw MutexException("Fifo mutex timed lock not implemented");
}

void FifoMutex::Unlock()
{
	_fifo_mutex.control.Lock();

	// Verify this thread is the owner, and that the fifo mutex is locked.
	if (!_fifo_mutex.active || _fifo_mutex.owner != pthread_self()) {
		_fifo_mutex.control.Unlock();

		// throw MutexException("Current thread does not own the mutex");
	}

	// Release the lock, letting the next thread lock the fifo mutex.
	try {
		_fifo_mutex.active->lock.Lock();
	} catch (jthread::MutexException &e) {
		_fifo_mutex.control.Unlock();
	}

	_fifo_mutex.active = NULL;
	_fifo_mutex.owner  = (pthread_t)-1;

	// Cleanup is done by the next thread locking the mutex.
	_fifo_mutex.control.Unlock();
}

bool FifoMutex::TryLock()
{
	struct jfifo_mutex_list_t *curr, *next, *temp;

	_fifo_mutex.control.Lock();

	// Get a new locked mutex.
	if (_fifo_mutex.unused) {
		// Reuse an unused one.
		curr = _fifo_mutex.unused;

		// If it is free, it is immediately lockable.
		if (curr->lock.TryLock() == false) {
			// No. Fail gracefully.
			_fifo_mutex.control.Unlock();

			return false;
		}

		// Good, detach it from the list.
		_fifo_mutex.unused = curr->next;
	} else {
		// Allocate a new one.
		curr = new struct jfifo_mutex_list_t;

		if (curr->lock.TryLock() == false) {
			// A new mutex is not lockable. WTF?
			delete curr;

			_fifo_mutex.control.Unlock();

			return false;
		}
	}

	// No previous owners?
	if (!_fifo_mutex.list) {
		// We own the fifo mutex now.
		_fifo_mutex.owner = pthread_self();
		_fifo_mutex.active = curr;
		// Add self to the mutex list.
		curr->next = NULL;
		_fifo_mutex.list = curr;

		// If there is more than one item in the free list, free the first one.
		if (_fifo_mutex.unused && _fifo_mutex.unused->next) {
			temp = _fifo_mutex.unused;
			_fifo_mutex.unused = temp->next;

			delete temp;
		}

		_fifo_mutex.control.Unlock();

		return true;
	}

	// There are previous owners. Add self to the list.
	next = _fifo_mutex.list;
	curr->next = next;
	_fifo_mutex.list = curr;

	if (next->lock.TryLock() == true) {
		// We obtained the lock immediately, so there were no waiting threads. 
		// If there is more than one item in the free list, free the first one.
		if (_fifo_mutex.unused && _fifo_mutex.unused->next) {
			temp = _fifo_mutex.unused;
			_fifo_mutex.unused = temp->next;

			delete temp;
		}
	} else {
		// Let other threads access the struture while we wait.
		_fifo_mutex.control.Unlock();

		// Take the previous owner mutex. This will block. If this fails, the 
		// previous owner has errored out somehow, and we own the mutex anyway. So ignore errors.
		next->lock.Lock();

		try {
			_fifo_mutex.control.Lock();
		} catch (jthread::MutexException *e) {
			// We failed to obtain the mutex protecting the structure, so we're essentially screwed. 
			// This should never happen, fortunately. Release our own mutex, and let a future thread 
			// worry about the cleanup.
			curr->lock.Unlock();

			return false;
		}

		// Okay, we're the owner.
	}

	// Mark self as the owner.
	_fifo_mutex.owner = pthread_self();
	_fifo_mutex.active = curr;

	// Release the predecessor lock; there is no-one else blocking on it.
	next->lock.Unlock();

	// Discard locks left over by predecessors.
	curr->next = NULL;

	while (next) {
		temp = next;
		next = next->next;
		temp->next = _fifo_mutex.unused;
		_fifo_mutex.unused = temp;
	}

	_fifo_mutex.control.Unlock();

	return true;
}

void FifoMutex::Trim()
{
	struct jfifo_mutex_list_t *curr, *next;

	_fifo_mutex.control.Lock();

	if (!_fifo_mutex.unused) {
		_fifo_mutex.control.Unlock();

		return;
	}

	next = _fifo_mutex.unused;
	_fifo_mutex.unused = NULL;

	while (next) {
		curr = next;
		next = next->next;
		curr->next = NULL;

		delete curr;
	}

	_fifo_mutex.control.Unlock();
}

int FifoMutex::CountUnused()
{
	struct jfifo_mutex_list_t *p;
	int n;

	_fifo_mutex.control.Lock();

	n = 0;
	p = _fifo_mutex.unused;

	while (p) {
		n++;
		p = p->next;
	}

	_fifo_mutex.control.Unlock();

	return n;
}

int FifoMutex::CountList()
{
	struct jfifo_mutex_list_t *p;
	int n;

	_fifo_mutex.control.Lock();

	n = 0;
	p = _fifo_mutex.list;

	while (p) {
		n++;
		p = p->next;
	}

	_fifo_mutex.control.Unlock();

	return n;
}

}
