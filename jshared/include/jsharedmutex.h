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

#include "jobject.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdint.h>
#endif

namespace jshared {

enum jmutex_flags_t {
#ifdef _WIN32
	MUTEX_CURRENT = 0,
	MUTEX_FUTURE = 0
#else
	MUTEX_CURRENT = MCL_CURRENT,
	MUTEX_FUTURE = MCL_FUTURE
#endif
};

/**
 * \brief SharedMutex.
 *
 * \author Jeff Ferr
 */
class SharedMutex : public virtual jcommon::Object{

    private:
		/** \brief Socket handler. */
#ifdef _WIN32
        int _id;
#else
        key_t _id;
#endif
        /** \brief */
		char *_shmp;
        /** \brief */
		int _memsize;
        /** \brief */
		bool _is_open;
        /** \brief */
		jmutex_flags_t _flags;

    public:
		/**
		 * \brief Constructor.
		 *
		 */
        SharedMutex(jmutex_flags_t flags_ = MUTEX_CURRENT | MUTEX_FUTURE);
	
		/**
		 * \brief Constructor.
		 *
		 */
        SharedMutex(void *data_, long long size_);
	
        /**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SharedMutex();
		
		/**
		 * \brief
		 *
		 */
		void Lock();
		
		/**
		 * \brief
		 *
		 */
		void Unlock();
		
		/**
		 * \brief
		 *
		 */
		void LockAll();
		
		/**
		 * \brief
		 *
		 */
		void UnlockAll();
		
		/**
		 * \brief 
		 *
		 */
		void Release();
        
};

}


#endif
