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
#include "jshared/jsharedmemory.h"
#include "jexception/jmemoryexception.h"

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SHARED_MEMORY_PATH "/dev/null"

namespace jshared {

SharedMemory::SharedMemory(int key, size_t size):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::SharedMemory");
  
  _size = size;
  _owner = false;

  if (_key = ftok(SHARED_MEMORY_PATH, key); _key == (key_t)-1) {
    throw jexception::MemoryException("ftok() for shm failed\n");
  }

  if (_id = shmget(_key, size, 0666); _id == -1) {
    throw jexception::MemoryException("shmget() failed\n");
  }

  if (_address = shmat(_id, NULL, 0); _address == nullptr) {
    throw jexception::MemoryException("shmat() failed\n");
  }
}

SharedMemory::SharedMemory(int key, size_t size, jshared_permissions_t perms):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::SharedMemory");

  int flags = 0;

  if ((perms & JSP_UR) != 0) {
    flags = flags | S_IRUSR;
  }
  
  if ((perms & JSP_UW) != 0) {
    flags = flags | S_IWUSR;
  }
  
  if ((perms & JSP_UX) != 0) {
    flags = flags | S_IXUSR;
  }
  
  if ((perms & JSP_GR) != 0) {
    flags = flags | S_IRGRP;
  }
  
  if ((perms & JSP_GW) != 0) {
    flags = flags | S_IWGRP;
  }
  
  if ((perms & JSP_GX) != 0) {
    flags = flags | S_IXGRP;
  }
  
  if ((perms & JSP_OR) != 0) {
    flags = flags | S_IROTH;
  }
  
  if ((perms & JSP_OW) != 0) {
    flags = flags | S_IWOTH;
  }
  
  if ((perms & JSP_OX) != 0) {
    flags = flags | S_IXOTH;
  }
  
  if ((perms & JSP_UID) != 0) {
    flags = flags | S_ISUID;
  }
  
  if ((perms & JSP_GID) != 0) {
    flags = flags | S_ISGID;
  }

  _size = size;
  _owner = true;

  if (_key = ftok(SHARED_MEMORY_PATH, key); _key == (key_t)-1) {
    throw jexception::MemoryException("ftok() for shm failed\n");
  }

  if (_id = shmget(_key, size, flags | IPC_CREAT | IPC_EXCL); _id == -1) {
    throw jexception::MemoryException("shmget() failed\n");
  }

  if (_address = shmat(_id, NULL, 0); _address == nullptr) {
    throw jexception::MemoryException("shmat() failed\n");
  }
}

SharedMemory::~SharedMemory()
{
  Release();
}

void * SharedMemory::GetAddress()
{
  return _address;
}

size_t SharedMemory::GetSize()
{
  return _size;
}

void SharedMemory::Release()
{
  if (shmdt(_address) == -1) {
    // throw jexception::MemoryException("shmdt() failed\n");
  }

  if (_owner == true) {
    struct shmid_ds 
      shmid_struct;

    if (shmctl(_id, IPC_RMID, &shmid_struct) == -1) {
      // throw jexception::MemoryException("shmctl() failed\n");
    }
  }
}

}

