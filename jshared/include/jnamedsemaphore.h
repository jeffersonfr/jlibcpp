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
#ifndef J_NAMEDSEMAPHORE_H
#define J_NAMEDSEMAPHORE_H

#include "jshared/jsharedlib.h"
#include "jcommon/jobject.h"

#include <iostream>

#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

namespace jshared {

struct jnamedsemaphore_t {
  pthread_mutex_t lock;
  pthread_cond_t nonzero;
  unsigned size;
  unsigned count;
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class NamedSemaphore : public virtual jcommon::Object {

  private:
    struct jnamedsemaphore_t *_handler;

  public:
    /**
     * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
     * only one process.
     *
     */
    NamedSemaphore(std::string name);

    /**
     * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
     * only one process.
     *
     */
    NamedSemaphore(std::string name, int access);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~NamedSemaphore();

    /**
     * \brief
     *
     */
    virtual void Wait();

    /**
     * \brief Wait milliseconds.
     *
     */
    virtual void Wait(uint64_t time_);

    /**
     * \brief .
     *
     */
    virtual void Notify();

    /**
     * \brief .
     *
     */
    virtual void NotifyAll();

    /**
     * \brief
     *
     */
    virtual void Release();

};

}

#endif
