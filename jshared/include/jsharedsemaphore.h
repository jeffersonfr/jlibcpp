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
#ifndef J_SHAREDSEMAPHORE_H
#define J_SHAREDSEMAPHORE_H

#include "jobject.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#endif

namespace jshared {

struct jsem_op_t {
	int *id;
	int length;
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SharedSemaphore : public virtual jcommon::Object{

    private:
		/** \brief */
		int _id;
		/** \brief */
		int _nsem;
		/** \brief Valor inicial de cada semaphoro */
		int _value;
		/** \brief */
		int _flag;

    public:
		/**
		 * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
		 * only one process.
		 *
		 */
#ifdef _WIN32
        SharedSemaphore(int key_ = 0);
#else
        // SharedSemaphore(key_t key_ = IPC_PRIVATE);
        SharedSemaphore(key_t key_ = 0);
#endif
	
		/**
		 * \brief Create a new semaphore.
		 *
		 */
#ifdef _WIN32
        SharedSemaphore(int key_, int nsem_ = 1, int value_ = 1, int perms_ = 0600);
#else
        // SharedSemaphore(key_t key_ = IPC_PRIVATE, int nsem_ = 1, int value_ = 1, int perms_ = 0600);
        SharedSemaphore(key_t key_ = 0, int nsem_ = 1, int value_ = 1, int perms_ = 0600);
#endif
		
        /**
		 * \brief Destrutor virtual.
	 	 *
	  	 */
	  	virtual ~SharedSemaphore();
		
		/**
		 * \brief
		 *
		 */
		void InitializeSemaphore();

		/**
		 * \brief
		 *
		 */
		void SetBlocking(bool b);
		
		/**
		 * \brief
		 *
		 */
		bool IsBlocking();
		
		/**
		 * \brief Wait milliseconds.
		 *
		 */
		void SetTimeout(int millis_, jsem_op_t *op = NULL);
		
		/**
		 * \brief
		 *
		 */
		void Wait(jsem_op_t *op = NULL);
		
		/**
		 * \brief
		 *
		 */
		void Notify(jsem_op_t *op = NULL);
		
		/**
		 * \brief
		 *
		 */
		void Release();

};

}

#endif
