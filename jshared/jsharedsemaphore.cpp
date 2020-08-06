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
#include "jshared/jsharedsemaphore.h"
#include "jexception/jsemaphoreexception.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jsemaphoretimeoutexception.h"

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SHARED_SEMAPHORE_PATH "/dev/null"

namespace jshared {

SharedSemaphore::SharedSemaphore(int key):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::SharedSemaphore");
  
  if ((_key = ftok(SHARED_SEMAPHORE_PATH, key)) == (key_t)-1) {
    throw jexception::SemaphoreException("ftok() for semaphore failed");
  }

  if (_id = semget(_key, 1, 0666); _id == -1) {
    throw jexception::SemaphoreException("semge() failed");
  }
}

SharedSemaphore::SharedSemaphore(int key, short value, jshared_permissions_t perms):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::SharedSemaphore");

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

  if (_key = ftok(SHARED_SEMAPHORE_PATH, key); _key == (key_t)-1) {
    throw jexception::SemaphoreException("ftok() for sem failed");
  }

  if (_id = semget(_key, 1, flags | IPC_CREAT | IPC_EXCL); _id == -1) {
    if (errno == EEXIST) {
      throw jexception::SemaphoreException("id already exists");
    }

    throw jexception::SemaphoreException("semget() failed\n");
  }

  short 
    values[1] = {value}; // INFO:: initial value (0)

  if (semctl(_id, 1, SETALL, values) == -1) {
    throw jexception::SemaphoreException("semctl() initialization failed\n");
  }
}

SharedSemaphore::~SharedSemaphore()
{
  Release();
}

void SharedSemaphore::SetBlocking(bool block)
{
  _blocking = block;
}

bool SharedSemaphore::IsBlocking()
{
  return _blocking;
}

void SharedSemaphore::Wait()
{
  struct sembuf 
    sops {
      .sem_num = 0,
      .sem_op = -1,
      .sem_flg = short((_blocking == true)?0:IPC_NOWAIT)
    };

  if (semop(_id, &sops, 1) == -1) {
    if (errno == EAGAIN) {
      if (_blocking == false) {
        throw jexception::SemaphoreException("No waiting exception");
      }
    }
      
    throw jexception::SemaphoreException("Shared semaphore wait failed");
  }
}

void SharedSemaphore::Wait(std::chrono::milliseconds ms)
{
  if (ms.count() < 0) {
    return;
  }

  struct sembuf 
    sops {
      .sem_num = 0,
      .sem_op = -1,
      .sem_flg = short((_blocking == true)?0:IPC_NOWAIT)
    };
  
  struct timespec 
    t;

  clock_gettime(CLOCK_REALTIME, &t);

  t.tv_sec += (int64_t)(ms.count()/1000LL);
  t.tv_nsec += (int64_t)(ms.count()%1000000000LL);

  if (semtimedop(_id, &sops, 1, &t) != 0) {
    if (errno == EAGAIN) {
      if (_blocking == false) {
        throw jexception::SemaphoreException("Shared semaphore no wait exception");
      } else {
        throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
      }
    }

    throw jexception::SemaphoreException("Shared semaphore wait failed");
  }
}

void SharedSemaphore::Notify(short count)
{
  if (count < 0) {
    return;
  }

  struct sembuf
    sops {
      .sem_num = 0,
      .sem_op = count,
      .sem_flg = short(0) // (_blocking == true)?0:IPC_NOWAIT
    };
  
  if (semop(_id, &sops, 1) != 0) {
    throw jexception::SemaphoreException("Shared semaphore notify failed");
  }
}

void SharedSemaphore::NotifyAll()
{
  int 
    count = semctl(_id, 0, GETNCNT, 0);

  if (count > 0) {
    Notify(count);
  }
}

int SharedSemaphore::GetValue()
{
  return semctl(_id, 0, GETNCNT, 0);
}

void SharedSemaphore::Release() 
{
  if (semctl(_id, 0, IPC_RMID) < 0) {
    throw jexception::SemaphoreException("Release shared semaphore error");
  }
}

}

