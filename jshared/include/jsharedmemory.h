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
#ifndef J_SHAREDMEMORY_H
#define J_SHAREDMEMORY_H

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

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SharedMemory : public virtual jcommon::Object{

    private:
        /** \brief */
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

    public:
		/**
		 * \brief Constructor.
		 *
		 */
#ifdef _WIN32
        SharedMemory(int key_ = 0, int memsize_ = 4096, int perms_ = 0600);
#else 
        SharedMemory(key_t key_ = 0, int memsize_ = 4096, int perms_ = 0600);
#endif
	
        /**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SharedMemory();
		
		/**
		 * \brief
		 *
		 */
		virtual void Attach();
		
		/**
		 * \brief
		 *
		 */
		virtual void Detach();
		
		/**
		 * \brief
		 *
		 */
		virtual void Deallocate();
		
		/**
		 * \brief
		 *
		 */
		virtual int Get(char *data_, int size_);
		
		/**
		 * \brief
		 *
		 */
		virtual int Put(const char *data_, int size_);
		
		/**
		 * \brief Close.
		 *
		 */
		virtual void Release();
        
};

}

#endif
