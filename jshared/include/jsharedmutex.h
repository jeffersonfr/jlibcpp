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
#ifndef J_SHAREDMUTEX_H
#define J_SHAREDMUTEX_H

#include "jshared/jsharedsemaphore.h"
#include "jshared/jmemorymap.h"

#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdint.h>

namespace jshared {

/**
 * \brief SharedMutex.
 *
 * \author Jeff Ferr
 */
class SharedMutex : public virtual jcommon::Object {

    private:
			/** \brief */
      MemoryMap *_mmap;

		public:
			/**
			 * \brief Constructor.
			 *
			 */
			SharedMutex(MemoryMap *mem);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~SharedMutex();

			/**
			 * \brief
			 *
			 */
			virtual void Lock();

			/**
			 * \brief
			 *
			 */
			virtual void Unlock();

			/**
			 * \brief
			 *
			 */
			virtual void LockAll();

			/**
			 * \brief
			 *
			 */
			virtual void UnlockAll();

};

}

#endif
