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

#include "jshared/jsharedlib.h"
#include "jcommon/jobject.h"

#include <iostream>
#include <thread>

#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/stat.h>

#include <unistd.h>

namespace jshared {

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SharedMemory : public virtual jcommon::Object {

  private:
    /** \brief */
    key_t _key;
    /** \brief */
    int _id;
    /** \brief */
    size_t _size;
    /** \brief */
    void *_address;
    /** \brief */
    bool _owner;

  public:
    /**
     * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
     * only one process.
     *
     */
    SharedMemory(key_t key, size_t size);

    /**
     * \brief Create a new semaphore.
     *
     */
    SharedMemory(key_t key, size_t size, jshared_permissions_t = JSP_URWX);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~SharedMemory();

    /**
     * \brief
     *
     */
    virtual void * GetAddress();

    /**
     * \brief
     *
     */
    virtual size_t GetSize();

    /**
     * \brief
     *
     */
    virtual void Release();

};

}

#endif
