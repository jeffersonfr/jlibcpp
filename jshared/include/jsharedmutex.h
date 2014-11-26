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

#include "jsharedsemaphore.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdint.h>
#endif

namespace jshared {

enum jsharedmutex_flags_t {
	JSF_CURRENT	= 1,
	JSF_FUTURE	= 2
};

/**
 * \brief SharedMutex.
 *
 * \author Jeff Ferr
 */
class SharedMutex : public virtual jcommon::Object{

    private:
			/** \brief . */
			jkey_t _id;
			/** \brief */
			int64_t _size;
			/** \brief */
			char *_shmp;
			/** \brief */
			bool _is_open;
			/** \brief */
			jsharedmutex_flags_t _flags;

		public:
			/**
			 * \brief Constructor.
			 *
			 */
			SharedMutex(jsharedmutex_flags_t flags_ = jsharedmutex_flags_t(JSF_CURRENT | JSF_FUTURE));

			/**
			 * \brief Constructor.
			 *
			 */
			SharedMutex(void *data_, int64_t size_);

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

			/**
			 * \brief 
			 *
			 */
			virtual void Release();

};

}

#endif
