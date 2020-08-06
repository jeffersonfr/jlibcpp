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
class SharedSemaphore : public virtual jcommon::Object {

  private:
    /** \brief */
    key_t _key;
    /** \brief */
    int _id;
    /** \brief */
    bool _blocking;

  public:
    /**
     * \brief Open a semaphore. IPC_PRIVATE parameter create a new semaphore for
     * only one process.
     *
     */
    SharedSemaphore(int key);

    /**
     * \brief Create a new semaphore.
     *
     */
    SharedSemaphore(int key, short value = 1, jshared_permissions_t = JSP_URWX);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~SharedSemaphore();

    /**
     * \brief
     *
     */
    virtual void Wait();

    /**
     * \brief Wait milliseconds.
     *
     */
    virtual void Wait(std::chrono::milliseconds ms);

    /**
     * \brief
     *
     */
    virtual void Notify(short count = 1);

    /**
     * \brief
     *
     */
    virtual void NotifyAll();

    /**
     * \brief
     *
     */
    virtual void Release();

    /**
     * \brief
     *
     */
    virtual void SetBlocking(bool block);

    /**
     * \brief
     *
     */
    virtual bool IsBlocking();

    /**
     * \brief
     *
     */
    virtual int GetValue();

};

}

#endif
