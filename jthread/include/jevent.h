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
#ifndef J_EVENT_H
#define J_EVENT_H

#include "jmutex.h"

namespace jthread {

class Event {
		
	private:
#ifdef _WIN32
		HANDLE _event;
#else
		jthread::Mutex _mutex;
		pthread_cond_t _condition;
		int _count;
		bool _signaled;
#endif

	public:
		/**
		 * \brief
		 *
		 */
		Event();

		/**
		 * \brief Destroys the Event.
		 */
		virtual ~Event();

		/**
		 * \brief Sets the Event to Non-Signalled state.
		 */
		virtual void Reset();

		/**
		 * \brief Unconditional wait. Waits for the Event to become Signalled. Wait returns immediately if 
		 * Event is already in Signalled state; the Event is automatically reset to Not-Signalled state.
		 *
		 */
		virtual void Wait();

		/**
		 * \brief Conditional wait. Waits for upto “secs” seconds for the Event to be signalled. Wait 
		 * returns immediately if Event is already in Signalled state; the Event is automatically reset to 
		 * Non-Signalled state.
		 *
		 */
		virtual void Wait(uint64_t secs);

		/**
		 * \brief Sets an Event to Signalled state, and wakes up one waiting thread.
		 *
		 */
		virtual void Notify();

		/**
		 * \brief Sets an Event to Signalled state, and wakes up one waiting thread.
		 *
		 */
		virtual void NotifyAll();

		/**
		 * \brief
		 *
		 */
		virtual void Release();

};

}

#endif

