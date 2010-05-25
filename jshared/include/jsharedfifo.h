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
#ifndef J_SHAREDFIFO_H
#define J_SHAREDFIFO_H

#include "jobject.h"

#include <iostream>

#include <stdint.h>

namespace jshared {

struct jshmhandle_t {
	int sid;
	void *mem;
	int privsz;		// size of single priv structure of upper program 
	int semid;		// semaphore id 
	int sz;			// size of shared mem in bytes 
};

struct jshmprefix_t {
	int counter;
	int read;		// offsets from shm->mem where we should read and write
	int write;		// min value for both is shm->privsz+sizeof(struct shmprefix)
};

struct jshmbh_t {
	int sz;
	unsigned canary;
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SharedFifo : public virtual jcommon::Object{

    private:
		struct jshmhandle_t *_shm;

		/**
		 * \brief
		 *
		 */
		void LLHRewind();
		
		/**
		 * \brief
		 *
		 */
		int LLMemFree();
		
		/**
		 * \brief
		 *
		 */
		int LLMemUsed();
		
		/**
		 * \brief
		 *
		 */
		int LLPut(void *data, int sz);
		
		/**
		 * \brief
		 *
		 */
		int LLGet(void *data, int sz);
		
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

    public:
		/**
		 * \brief Constructor. 
		 *
		 */
#ifdef _WIN32
        SharedFifo(int key_, int npages_, int struct_size_);
#else
        SharedFifo(key_t key_, int npages_, int struct_size_);
#endif
	
        /**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SharedFifo();
		
		/**
		 * \brief
		 *
		 */
		void Attach();
		
		/**
		 * \brief
		 *
		 */
		void Dealloc();
		
		/**
		 * \brief
		 *
		 */
		void Detach();
		
		/**
		 * \brief
		 *
		 */
		void Setpriv(void *priv);
		
		/**
		 * \brief
		 *
		 */
		void Getpriv(void *priv);
		
		/**
		 * \brief
		 *
		 */
		int Get(void *data,int sz);
		
		/**
		 * \brief
		 *
		 */
		int Put(void *data, int sz);
		
		/**
		 * \brief
		 *
		 */
		bool IsEmpty();
		
		/**
		 * \brief Close.
		 *
		 */
		void Close();
		
};

};

#endif
