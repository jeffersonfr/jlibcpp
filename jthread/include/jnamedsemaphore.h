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
#ifndef J_NAMEDSEMAPHORE_H
#define J_NAMEDSEMAPHORE_H

#include "jmutex.h"

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
 * \brief NamedSemaphore.
 *
 * @author Jeff Ferr
 */
class NamedSemaphore : public virtual jcommon::Object{

    private:
#ifdef _WIN32
#else
			/** \brief NamedSemaphore handler. */
			sem_t *_handler;
#endif
			/** \brief */
			Mutex _mutex;
			/** \brief */
			int _counter;

    public:
			/**
			 * \brief Construtor.
			 *
			 */
			NamedSemaphore(std::string file, int mode, int value = 0);

			/**
			 * \brief Construtor.
			 *
			 */
			NamedSemaphore(std::string file);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~NamedSemaphore();

			/**
			 * \brief Lock the semaphore.
			 *
			 */
			void Wait();

			/**
			 * \brief Lock semaphore.
			 *
			 */
			void Wait(uint64_t time_);

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
