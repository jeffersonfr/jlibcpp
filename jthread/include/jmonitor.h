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
#ifndef J_MONITOR_H
#define J_MONITOR_H

#include "jmutex.h"
#include "jcondition.h"

#include <map>

namespace jthread {

struct jmonitor_map_t {
	Condition sem;
	bool signalled;
};

class Monitor {

	private:
		jmonitor_map_t * GetCurrentThread();

	protected:
		/** \brief */
		Mutex _mutex;
		/** \brief */
		jmonitor_map_t *_holder;		
		/** \brief */
		int _lock_count;
		/** \brief */
		std::vector<jmonitor_map_t *> waitQ,
			readyQ;

#ifdef _WIN32
		std::map<HANDLE, jmonitor_map_t *> threadQ;
#else
		std::map<pthread_t, jmonitor_map_t *> threadQ;
#endif

	protected:
		/** 
		 * \brief
		 *
		 */
		// Monitor(const Monitor&);
		
		/** 
		 * \brief
		 *
		 */
		Monitor & operator=(const Monitor&);

		/** 
		 * \brief
		 *
		 */
		void GrantMonitor();

	public:
		/**
		 * \brief Creates a Monitor Object.
		 *
		 */
		Monitor();

		/**
		 * \brief Destroys a Monitor object.
		 *
		 */
		virtual ~Monitor();

		/**
		 * \brief Requests a lock on the Monitor object. Will block if Lock is not available.
		 *
		 */
		void Enter();

		/**
		 * \brief Requests a lock on the Monitor object.  Will return false if Lock is not 
		 * available. Returns true if lock was acquired.
		 *
		 */
		bool TryEnter();

		/** 
		 * \brief Requester must be a holder of the lock. If the Unlock succeeds, the lock is 
		 * granted to the oldest waiting thread on the ReadyQ.
		 *
		 */
		void Exit();

		/**
		 * \brief Requester must be a holder of the lock. The lock is released and requester put 
		 * to sleep until the Monitor is signalled. Lock is re-acquired before control returns to 
		 * the caller.
		 */
		void Wait();

		/**
		 * \brief Requester must be a holder of the lock. The lock is released and requester put 
		 * to sleep until the Monitor is signalled or the timeout expires. Lock is re-acquired 
		 * before control returns to the caller. 
		 */
		bool Wait(uint64_t secs);

		/**
		 * \brief Signals and wakes up one thread waiting on the Monitor object. The thread woken 
		 * up may not be the one signalled.
		 *
		 */
		void Notify();

		/**
		 * \brief Signalls all waiting threads and wakes up one thread.
		 */
		void NotifyAll();

};

}

#endif

