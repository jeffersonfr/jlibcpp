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
#ifndef J_FIFOMUTEX_H
#define J_FIFOMUTEX_H

#include "jmutex.h"
#include "jthread.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace jthread {

struct jfifo_mutex_list_t {
	Mutex lock;
	struct jfifo_mutex_list_t *next;
};

struct jfifo_mutex_t {
	// Lock protecting all fields
	Mutex control;
	// Owner identity
	jthread_t owner;
	// List of unused locks (allocation cache)
	struct jfifo_mutex_list_t *unused;
	// Mutex list
	struct jfifo_mutex_list_t *list;
	// Pointer to the active mutex in the list (tail)
	struct jfifo_mutex_list_t *active;
};

/**
* \brief Monitor.
*
* @author Jeff Ferr
*/
class FifoMutex : public virtual jcommon::Object{

	private:
		struct jfifo_mutex_t _fifo_mutex;

	public:
		/**
		 * \brief Initialize the FIFO mutex.
		 *
		 * The second parameter is the pthread_mutexattr_t pointer, which defines 
		 * the attributes for the structure-locking mutex; it is recommended to 
		 * always supply NULL.
		 *
		 */
		FifoMutex();

		/**
		 * \brief Destroy the FIFO mutex.
		 *
		 */
		virtual ~FifoMutex();

		/**
		 * \brief Verify if a section is locked.
		 *
		 */
		virtual bool IsLocked();

		/**
		 * \brief Lock the critial section.
		 *
		 */
		virtual void Lock();

		/**
		 * \brief Lock the critial section.
		 *
		 */
		virtual void Lock(int time_);

		/**
		 * \brief Unlock the critical section.
		 *
		 */
		virtual void Unlock();

		/**
		 * \brief Try lock the critical section.
		 *
		 */
		virtual bool TryLock();

		/*
		 * \brief Remove cached entries from the FIFO mutex. When contended, 
		 * the FIFO mutex will grow automatically. When uncontended, the 
		 * cached entries will be slowly released. You can use this function 
		 * to remove all unused cached entries from the mutex. There should 
		 * be no need to call this function during normal program operation, 
		 * but it can be called at any time.
		 */
		virtual void Trim();

		/*
		 * \brief Return the number of unused entries cached in the FIFO mutex.
		 */
		virtual int CountUnused();

		/* 
		 * \brief Return the number of entries in the FIFO mutex list.
		 * Since cleanup is done by the next thread to lock the mutex,
		 * the result includes both the number of threads blocked on
		 * this FIFO mutex, plus the number of threads to clean up.
		 * If the current thread holds the FIFO mutex, then the count
		 * is 1 + the number of other threads blocking on the mutex.
		 */
		virtual int CountList();

};

}

#endif
