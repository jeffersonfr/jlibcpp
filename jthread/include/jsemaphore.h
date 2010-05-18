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
#ifndef J_SEMAPHORE_H
#define J_SEMAPHORE_H

#include "jthread.h"

#include "jobject.h"
#include "jautolock.h"

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <winbase.h>
#include <string>
#else
#include <sys/types.h>
#include <semaphore.h>
#endif

#include <stdint.h>

namespace jthread {

/**
 * \brief Semaphore.
 *
 * @author Jeff Ferr
 */
class Semaphore : public virtual jcommon::Object{

    private:
#ifdef _WIN32
			/** \brief */
			std::string _semaphore_name;
			/** \brief */
			HANDLE _semaphore;
			/** \brief */
			LPSECURITY_ATTRIBUTES _sa;
			/** \brief */
			BOOL _inherit;   
#else
			/** \brief Semaphore handler. */
			sem_t _semaphore;
#endif
			/** \brief */
			Mutex mutex;

    public:
			/**
			 * \brief Construtor.
			 *
			 */
			Semaphore(int = 0);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~Semaphore();

			/**
			 * \brief Lock the semaphore.
			 *
			 */
			void Wait();

			/**
			 * \brief Lock semaphore.
			 *
			 */
			void Wait(long long time_);

			/**
			 * \brief Notify the locked semaphore.
			 *
			 */
			void Notify();

			/**
			 * \brief Notify all locked semaphores.
			 *
			 */
			void NotifyAll();

			/**
			 * \brief Try lock the semaphore.
			 *
			 */
			bool TryWait();

			/**
			 * \brief
			 *
			 */
			int GetValue();

			/**
			 * \brief
			 *
			 */
			void Release();

};

}

#endif
