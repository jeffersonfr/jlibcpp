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

#include "jobject.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace jthread {

enum jmutex_type_t {
    FAST_MUTEX,
    RECURSIVE_MUTEX,
    ERROR_CHECK_MUTEX
};

enum jmutex_protocol_t {
	NONE_PROT_MUTEX,
	INHERIT_PROT_MUTEX,
	PROTECT_PROT_MUTEX
};

/**
* \brief Monitor.
*
* @author Jeff Ferr
*/
class Mutex : public virtual jcommon::Object{

friend class Condition;

private:
#ifdef _WIN32
	HANDLE _mutex;
#else
	/** \brief */
	pthread_mutex_t _mutex;
#endif
	/** \brief */
	jmutex_type_t _type;
	/** \brief */
	int _lock_count;

public:
	/**
	 * \brief Construtor.
	 *
	 */
	Mutex(jmutex_type_t = ERROR_CHECK_MUTEX, jmutex_protocol_t protocol_ = NONE_PROT_MUTEX, bool block_in_death = false);

	/**
	 * \brief Destrutor virtual.
	 *
	 */
	virtual ~Mutex();

	/**
	 * \brief Verify if a section is locked.
	 *
	 */
	bool IsLocked();

        /**
         * \brief Lock the critial section.
         *
         */
        void Lock();
        
        /**
         * \brief Unlock the critical section.
         *
         */
        void Unlock();
        
        /**
         * \brief Try lock the critical section.
         *
         */
        bool TryLock();
    
		/**
		 * \brief
		 *
		 */
		std::string what();
};

}

#endif
