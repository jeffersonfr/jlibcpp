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

#include "jshared/jsharedlib.h"
#include "jcommon/jobject.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_PACKET_SIZE  256

namespace jshared {

struct jpage_msg_t {
  long mtype;
  char msg[MAX_PACKET_SIZE];
};

struct msgbuf {
  int mtype;
};

union msg_t {
  struct msgbuf buf;
  jpage_msg_t msg;
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class MessageQueue : public virtual jcommon::Object{

    private:
      /** \brief */
      key_t _id;
      /** \brief */
      bool _is_open;
      /** \brief */
      bool _blocking;

  public:
    /**
     * \brief Constructor.
     *
     */
    MessageQueue(key_t key_ = JIPC_PRIVATE, int perms_ = 0600, int size_ = MAX_PACKET_SIZE, int maxmsgs_ = 0);
  
    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~MessageQueue();
    
    /**
     * \brief
     *
     */
    virtual int GetMessageIndex();
    
    /**
     * \brief
     *
     */
    virtual void Send(const void *data_, int size_, long type_ = 1);
    
    /**
     * \brief
     *
     */
    virtual void Receive(void *data_, int size_, long mtype_ = 0, int flag_ = 0);
    
    /**
     * \brief
     * 
     */
    virtual void SetBlocking(bool b_);
    
    /**
     * \brief Close.
     *
     */
    virtual void Release();
    
};

}

#endif
