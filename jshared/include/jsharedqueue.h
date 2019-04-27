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
#ifndef J_SHAREDQUEUE_H
#define J_SHAREDQUEUE_H

#include "jshared/jsharedsemaphore.h"

#include <iostream>

#include <sys/shm.h>
#include <sys/ipc.h>

namespace jshared {

struct jshmhandle_t {
  int sid;
  void *mem;
  int privsz;    // size of single priv structure of upper program 
  int semid;    // semaphore id 
  int sz;      // size of shared mem in bytes 
};

struct jshmprefix_t {
  int counter;
  int read;    // offsets from shm->mem where we should read and write
  int write;    // min value for both is shm->privsz+sizeof(struct shmprefix)
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
class SharedQueue : public virtual jcommon::Object {

  private:
    /** \brief */
    struct jshmhandle_t *_shm;

  private:
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
    SharedQueue(key_t key_, int npages_, int struct_size_);
  
    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~SharedQueue();
    
    /**
     * \brief
     *
     */
    virtual void Attach();
    
    /**
     * \brief
     *
     */
    virtual void Deallocate();
    
    /**
     * \brief
     *
     */
    virtual void Detach();
    
    /**
     * \brief
     *
     */
    virtual void Setpriv(void *priv);
    
    /**
     * \brief
     *
     */
    virtual void Getpriv(void *priv);
    
    /**
     * \brief
     *
     */
    virtual int Get(void *data,int sz);
    
    /**
     * \brief
     *
     */
    virtual int Put(void *data, int sz);
    
    /**
     * \brief
     *
     */
    virtual bool IsEmpty();
    
    /**
     * \brief Close.
     *
     */
    virtual void Close();
    
};

}

#endif
