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
#ifndef J_MESSAGEQUEUE_H
#define J_MESSAGEQUEUE_H

#include "jobject.h"

#ifdef _WIN32
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

#include <stdint.h>

#define MAX_PACKET_SIZE	256

namespace jshared {

struct jpage_msg_t {
	long mtype;
	char msg[MAX_PACKET_SIZE];
};

#ifdef __CYGWIN32__
struct msgbuf {
	int mtype;
};
#endif

union msg_t {
#ifdef _WIN32
	jpage_msg_t msg;
#else
	struct msgbuf buf;
	jpage_msg_t msg;
#endif
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class MessageQueue : public virtual jcommon::Object{

    private:
        /** \brief */
#ifdef _WIN32
		HANDLE _id;
#else
        key_t _id;
#endif
        /** \brief */
		bool _is_open;
        /** \brief */
		bool _blocking;

	public:
		/**
		 * \brief Constructor.
		 *
		 */
#ifdef _WIN32
		MessageQueue(int key_ = 0, int perms_ = 0600, int maxsize_ = MAX_PACKET_SIZE, int maxmsgs_ = 0);
#else
		// MessageQueue(key_t key_ = IPC_PRIVATE, int perms_ = 0600, int size_ = MAX_PACKET_SIZE, int maxmsgs_ = 0);
		MessageQueue(key_t key_ = 0, int perms_ = 0600, int size_ = MAX_PACKET_SIZE, int maxmsgs_ = 0);
#endif
	
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~MessageQueue();
		
		/**
		 * \brief
		 *
		 */
		int GetMessageIndex();
		
		/**
		 * \brief
		 *
		 */
		void Send(const void *data_, int size_, long type_ = 1);
		
		/**
		 * \brief
		 *
		 */
		void Receive(void *data_, int size_, long mtype_ = 0, int flag_ = 0);
		
		/**
		 * \brief
		 * 
		 */
		void SetBlocking(bool b_);
		
		/**
		 * \brief Close.
		 *
		 */
		void Release();
		
};

}

#endif
