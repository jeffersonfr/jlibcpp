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
#ifndef J_SPINLOCK_H
#define J_SPINLOCK_H

#include "jobject.h"

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <winbase.h>
#include <string>
#else
#endif

namespace jthread {

/**
 * \brief Semaphore.
 *
 * @author Jeff Ferr
 */
class SpinLock : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		volatile unsigned long _lock;
#else
		pthread_spinlock_t _lock;
#endif

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		SpinLock();

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SpinLock();

		/**
		 * \brief Lock the semaphore.
		 *
		 */
		virtual void Lock();

		/**
		 * \brief Notify the locked semaphore.
		 *
		 */
		virtual void Unlock();

		/**
		 * \brief Try lock the semaphore.
		 *
		 */
		virtual bool TryLock();

		/**
		 * \brief
		 *
		 */
		virtual void Release();

};

}

#endif
