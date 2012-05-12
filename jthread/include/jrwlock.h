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
#ifndef J_RWLOCK_H
#define J_RWLOCK_H

#include "jobject.h"

#include <string>

#ifdef _WIN32
#include <Windows.h>
#include <WinBase.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace jthread {

/**
* \brief Monitor.
*
* @author Jeff Ferr
*/
class RWLock : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		SRWLock _rwlock;
#else
		/** \brief */
		pthread_rwlock_t _rwlock;
#endif

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		RWLock();

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~RWLock();

		/**
		 * \brief Lock the critial section.
		 *
		 */
		void ReadLock();

		/**
		 * \brief Lock the critial section.
		 *
		 */
		void WriteLock();

		/**
		 * \brief Unlock the critical section.
		 *
		 */
		void ReadUnlock();

		/**
		 * \brief Unlock the critical section.
		 *
		 */
		void WriteUnlock();

		/**
		 * \brief Try lock the critical section.
		 *
		 */
		bool TryReadLock();

		/**
		 * \brief Try lock the critical section.
		 *
		 */
		bool TryWriteLock();

};

}

#endif
