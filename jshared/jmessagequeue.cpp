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
#include "Stdafx.h"
#include "jmessagequeue.h"
#include "jmessageexception.h"

namespace jshared {

#ifdef _WIN32
MessageQueue::MessageQueue(int key_, int perms_, int maxsize_, int maxmsgs_):
#else 
MessageQueue::MessageQueue(key_t key_, int perms_, int size_, int maxmsgs_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::MessageQueue");
	
#ifdef _WIN32
	/* TODO::
	MSGQUEUEOPTIONS_OS desc;

	if (maxsize_ < 4) {
		maxsize_ = 4;
	}

	if (maxmsgs_ < 0) {
		maxmsgs_ = 0;
	}

	desc.dwSize = maxsize_;
	desc.dwFlags = MSGQUEUE_NOPRECOMMIT; // MSGQUEUE_ALLOW_BROKEN
	desc.dwMaxMessages = maxmsgs_;
	desc.cbMaxMessage = maxsize_;
	desc.bReadAccess = ((paerms_ & 0444) != 0)?TRUE:FALSE;

	_id = CreateMsgQueue(key_.c_str(), &desc);
	*/
#else
	_id = msgget(key_, IPC_CREAT | IPC_EXCL | perms_);

	if (_id < 0) {
		_id = msgget(key_, 0);
		
		if (_id < 0) {
			if (errno == EACCES) {
				throw MessageException("No access permissions to the queue");
			} else {
				throw MessageException("Error opening a message queue");
			}
		}
	}
#endif
	
	_is_open = true;
	_blocking = true;
}

MessageQueue::~MessageQueue()
{
}

int MessageQueue::GetMessageIndex()
{
#ifdef _WIN32
	/* TODO::
	MSGQUEUEINFO info;

	GetMsgQueueInfo(_id, &info);

	return info.dwCurrentMessages;
	*/

	return -1;
#else
	struct msqid_ds buf;
	
	int r = msgctl (_id, IPC_STAT, &buf);

	if (r < 0) {
		buf.msg_qnum = 0;
	}
	
	return buf.msg_qnum;
#endif
}

void MessageQueue::Send(const void *msg_, int size_, long type_)
{
    if (_is_open == false) {
		throw MessageException("Message queue is released");
    }

	if (msg_ == NULL) {
		throw MessageException("Null pointer exception");
	}

	if (type_ <= 0) {
		type_ = 1;
	}
	
#ifdef _WIN32
	/* TODO::
	msg_t msg;

	msg.msg.mtype = type_;
	memcpy(&(msg.msg.msg), msg_, size_);
	
	BOOL b = WriteMsgQueue(
		_id,
		msg.buf,
		sizeof(msg_t) - sizeof(long),
		INFINITE,
		MSGQUEUE_MSGALERT 
	);

	if (b == FALSE) {
		throw MessageException("Send message error");
	}
	*/
#else
	msg_t msg;

	msg.msg.mtype = type_;
	memcpy(&(msg.msg.msg), msg_, size_);
	
	int r, 
		msz = sizeof(msg_t) - sizeof(long);

	if (_blocking == true) {
		r = msgsnd(_id, &msg.buf, msz, 0);
	} else {
		r = msgsnd(_id, &msg.buf, msz, IPC_NOWAIT);
	}

	if (r < 0) {
		if (errno == EAGAIN) {
			throw MessageException("Buffer is fully");
		} else {
			throw MessageException("Send message error");
		}
	}
#endif
}

void MessageQueue::Receive(void *msg_, int size_, long type_, int flag_)
{
    if (_is_open == false) {
		 throw MessageException("Message queue is released");
    }

	if (msg_ == NULL) {
		 throw MessageException("Null pointer exception");
	}
	
#ifdef _WIN32
	/* TODO::
	BOOL b;
	int r;
	msg_t msg;

	msg.buf.mtype = type_;

	b= ReadMsgQueue(
		_id,
		&msg.buf,
		sizeof(msg_t) - sizeof(long),
		&r,
		INFINITE,
		MSGQUEUE_MSGALERT
	);

	if (b == FALSE) {
		throw MessageException("Receive message error");
	}
	*/
#else
	msg_t msg;

	msg.buf.mtype = type_;
	
	int r,
		msz = sizeof(msg_t) - sizeof(long);
	
	do {
		if (_blocking == true) {
			r = msgrcv(_id, &msg.buf, msz, type_, 0 | flag_);
		} else {
			r = msgrcv(_id, &msg.buf, msz, type_, IPC_NOWAIT | flag_);
		}
	} while (r == -1 && errno == EINTR);

	if (r < 0) {
		msg_ = 0;
		
		if (errno == ENOMSG) {
			throw MessageException("Receive message error");
		}
	}

	memcpy(msg_, &(msg.msg.msg), size_);
#endif
}

void MessageQueue::SetBlocking(bool b_)
{
	_blocking = b_;
}

void MessageQueue::Release()
{
#ifdef _WIN32
	/* TODO::
	BOOL b = CloseMsgQueue(_id);

	if (b == FALSE) {
		throw MessageException("Release message queue failed");
	}
	*/
#else
	int r = msgctl(_id, IPC_RMID, 0);

	if (r < 0) {
		if (errno == EACCES) {
			throw MessageException("No access permissions to remove the queue");
		} else if (errno == EIDRM) {
			throw MessageException("Message queue was removed");
		} else {
			throw MessageException("Error releasing the message queue");
		}
	}
#endif
}

}
