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

#include "jsharedlib.h"
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

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SharedSemaphore : public virtual jcommon::Object{

	class SharedSemaphoreOp {

		private:
			/** \brief */
			int _id;
			/** \brief */
			int _index;
			/** \brief */
			bool _is_blocking;

		public:
			/**
			 * \brief
			 *
			 */
			SharedSemaphoreOp(int id, int index);

			/**
			 * \brief
			 *
			 */
			virtual ~SharedSemaphoreOp();

			/**
			 * \brief
			 *
			 */
			virtual void SetBlocking(bool b);

			/**
			 * \brief
			 *
			 */
			virtual bool IsBlocking();

			/**
			 * \brief
			 *
			 */
			virtual int GetLocked();

			/**
			 * \brief
			 *
			 */
			virtual int GetUnlocked();

			/**
			 * \brief
			 *
			 */
			virtual void Wait();

			/**
			 * \brief Wait milliseconds.
			 *
			 */
			virtual void Wait(int ms);

			/**
			 * \brief
			 *
			 */
			virtual void Notify(int n);

			/**
			 * \brief
			 *
			 */
			virtual void NotifyAll();

	};

	private:
		/** \brief */
		jkey_t _key;
		/** \brief */
		int _id;
		/** \brief */
		int _nsem;
		/** \brief */
		bool _is_blocking;

	public:
		/**
		 * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
		 * only one process.
		 *
		 */
		SharedSemaphore(jkey_t key_);

		/**
		 * \brief Create a new semaphore.
		 *
		 */
		SharedSemaphore(jkey_t key_, int nsem_, int value_ = 1, jshared_permissions_t = JSP_URWX);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SharedSemaphore();

		/**
		 * \brief
		 *
		 */
		virtual jkey_t GetKey();

		/**
		 * \brief
		 *
		 */
		virtual SharedSemaphoreOp At(int index);

		/**
		 * \brief
		 *
		 */
		virtual void SetBlocking(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsBlocking();

		/**
		 * \brief
		 *
		 */
		virtual void Wait(int *array, int array_size);

		/**
		 * \brief Wait milliseconds.
		 *
		 */
		virtual void Wait(int *array, int array_size, int ms);

		/**
		 * \brief
		 *
		 */
		virtual void Notify(int *array, int array_size, int n);

		/**
		 * \brief
		 *
		 */
		virtual void NotifyAll(int *array, int array_size);

		/**
		 * \brief
		 *
		 */
		virtual void Release();

};

}

#endif
