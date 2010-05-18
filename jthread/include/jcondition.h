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
#ifndef J_CONDITION_H
#define J_CONDITION_H

#include "jmutex.h"

#include "jobject.h"

#ifdef _WIN32
#else
#include <pthread.h>
#include <stdint.h>
#endif

namespace jthread {

/**
 * \brief Condition.
 *
 * @author Jeff Ferr
 */
class Condition : public virtual jcommon::Object{

    private:
#ifdef _WIN32
			Semaphore _semaphore;
#else
			/** \brief Mutex */
			Mutex _monitor;
			/** \brief Descritor */
			pthread_cond_t _condition;
#endif

    public:
			/**
			 * \brief Construtor.
			 *
			 */
			Condition(int nblock = 1);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~Condition();

			/**
			 * \brief Lock semaphore.
			 *
			 */
			void Wait(Mutex *mutex = NULL);

			/**
			 * \brief Lock semaphore.
			 *
			 * \param time_ is an absolute time specification, zero is the beginning of 
			 * the epoch (00:00:00 GMT, January 1, 1970).
			 *
			 */
			void Wait(long long time_, Mutex *mutex = NULL);

			/**
			 * \brief Notify the locked semaphore.
			 *
			 */
			void Notify();

			/**
			 * \brief Notify all the locked semaphores.
			 *
			 */
			void NotifyAll();

			/**
			 * \brief
			 *
			 */
			void Release();

};

}

#endif
