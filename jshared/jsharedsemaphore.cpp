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

namespace jshared {

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
	// union semun is defined by including <sys/sem.h>
#else
	// according to X/OPEN we have to define it ourselves
union semun {
	int val;                  // value for SETVAL
	struct semid_ds *buf;     // buffer for IPC_STAT, IPC_SET
	unsigned short *array;    // array for GETALL, SETALL
	struct seminfo *__buf;    // buffer for IPC_INFO
};
#endif

SharedSemaphore::SharedSemaphore(key_t key_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedSemaphore");
	
	_key = key_;
	_nsem = 0;
	_is_blocking = true;

	_id = semget(key_, 0, 0);

	if (_id < 0) {
		throw jexception::SemaphoreException("Open shared semaphore error");
	}

	// TODO:: initialize _nsem
}

SharedSemaphore::SharedSemaphore(key_t key_, int nsem_, int value_, jshared_permissions_t perms_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedSemaphore");

	if (nsem_ <= 0) {
		throw jexception::SemaphoreException("Number of semaphores must be greater than 0");
	}

	_key = key_;
	_nsem = nsem_;
	_is_blocking = true;
	
	int flags = 0;

	if ((perms_ & JSP_UR) != 0) {
		flags = flags | S_IRUSR;
	}
	
	if ((perms_ & JSP_UW) != 0) {
		flags = flags | S_IWUSR;
	}
	
	if ((perms_ & JSP_UX) != 0) {
		flags = flags | S_IXUSR;
	}
	
	if ((perms_ & JSP_GR) != 0) {
		flags = flags | S_IRGRP;
	}
	
	if ((perms_ & JSP_GW) != 0) {
		flags = flags | S_IWGRP;
	}
	
	if ((perms_ & JSP_GX) != 0) {
		flags = flags | S_IXGRP;
	}
	
	if ((perms_ & JSP_OR) != 0) {
		flags = flags | S_IROTH;
	}
	
	if ((perms_ & JSP_OW) != 0) {
		flags = flags | S_IWOTH;
	}
	
	if ((perms_ & JSP_OX) != 0) {
		flags = flags | S_IXOTH;
	}
	
	if ((perms_ & JSP_UID) != 0) {
		flags = flags | S_ISUID;
	}
	
	if ((perms_ & JSP_GID) != 0) {
		flags = flags | S_ISGID;
	}

	_id = semget(key_, _nsem, (int)(IPC_CREAT | IPC_EXCL | flags));

	if (_id < 0) {
		if (errno == EEXIST) {
			throw jexception::SemaphoreException("Shared semaphore already created");
		}

		throw jexception::SemaphoreException("Shared semaphore create failed");
	}

	/*
	// INFO:: initialize semaphore
	uint16_t *initv = new uint16_t[_nsem];
	int r;

	for (int i=0; i<_nsem; i++) {
		initv[i] = value_;
	}
	
	union semun arg;

	arg.array = initv;

	r = semctl(_id, _nsem, SETALL, arg);

	delete [] initv;
	
	if (r < 0) {
		throw jexception::SemaphoreException(strerror(errno));
	}
	*/
}

SharedSemaphore::~SharedSemaphore()
{
	Release();
}

SharedSemaphore::SharedSemaphoreOp SharedSemaphore::At(int index)
{
	if (index < 0 || index >= _nsem) {
		throw jexception::OutOfBoundsException("Index of shared semaphore out of bounds");
	}

	return SharedSemaphoreOp(_id, index);
}

SharedSemaphore::SharedSemaphoreOp::SharedSemaphoreOp(int id, int index)
{
	_id = id;
	_index = index;
	_is_blocking = true;
}

SharedSemaphore::SharedSemaphoreOp::~SharedSemaphoreOp()
{
}

void SharedSemaphore::SharedSemaphoreOp::SetBlocking(bool b)
{
	_is_blocking = b;
}

bool SharedSemaphore::SharedSemaphoreOp::IsBlocking()
{
	return _is_blocking;
}

void SharedSemaphore::SharedSemaphoreOp::Wait()
{
	struct sembuf sops;

	sops.sem_num = _index;
	sops.sem_op = -1;
	sops.sem_flg = SEM_UNDO;

	if (_is_blocking == false) {
		sops.sem_flg |= IPC_NOWAIT;
	}
	
	if (semop(_id, &sops, 1) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore wait failed");
		}
	}
	
	semctl(_id, _index, SETVAL, semctl(_id, _index, GETVAL, 0));
}

void SharedSemaphore::SharedSemaphoreOp::Wait(int time_)
{
	if (time_ < 0) {
		return;
	}

	struct sembuf sops;

	sops.sem_num = _index;
	sops.sem_op = -1;
	sops.sem_flg = SEM_UNDO;

	if (_is_blocking == false) {
		sops.sem_flg |= IPC_NOWAIT;
	}
	
	struct timespec t;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	if (semtimedop(_id, &sops, 1, &t) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore wait failed");
		}
	}
	
	semctl(_id, _index, SETVAL, semctl(_id, _index, GETVAL, 0));
}

void SharedSemaphore::SharedSemaphoreOp::Notify(int n)
{
	if (n < 0) {
		throw jexception::OutOfBoundsException("Notify's parameter is out of bounds");
	}

	struct sembuf sops;
	
	sops.sem_num = _index;
	sops.sem_op = n;
	sops.sem_flg = SEM_UNDO;
	
	if (_is_blocking == false) {
		sops.sem_flg |= IPC_NOWAIT;
	}
	
	if (semop(_id, &sops, 1) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore notify failed");
		}
	}
			
	semctl(_id, _index, SETVAL, semctl(_id, _index, GETVAL, 0));			
}

void SharedSemaphore::SharedSemaphoreOp::NotifyAll()
{
	int count = semctl(_id, _index, GETNCNT, 0);

	if(count > 0) {
		Notify(count);
	}
}

int SharedSemaphore::SharedSemaphoreOp::GetLocked()
{
	return semctl(_id, _index, GETNCNT, 0);
}

int SharedSemaphore::SharedSemaphoreOp::GetUnlocked()
{
	return semctl(_id, _index, GETZCNT, 0);
}

void SharedSemaphore::SetBlocking(bool b)
{
	_is_blocking = b;
}

bool SharedSemaphore::IsBlocking()
{
	return _is_blocking;
}

void SharedSemaphore::Wait(int *array, int array_size)
{
	int sz = _nsem;
	bool param = false;

	if (array != NULL && array_size > 0) {
		sz = array_size;
		param = true;
	}
		
	struct sembuf sops[sz];

	for (int i=0; i<sz; i++) {
		if (param == false) {
			sops[i].sem_num = i;
		} else {
			sops[i].sem_num = array[i];
		}

		sops[i].sem_op = -1;
		sops[i].sem_flg = SEM_UNDO;

		if (_is_blocking == false) {
			sops[i].sem_flg |= IPC_NOWAIT;
		}
	}

	if (semop(_id, sops, _nsem) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore wait failed");
		}
	}
	
	semctl(_id, 0, SETALL, semctl(_id, 0, GETALL, 0));			
}

void SharedSemaphore::Wait(int *array, int array_size, int time_)
{
	if (time_ < 0) {
		return;
	}

	int sz = _nsem;
	bool param = false;

	if (array != NULL && array_size > 0) {
		sz = array_size;
		param = true;
	}
		
	struct sembuf sops[sz];

	for (int i=0; i<sz; i++) {
		if (param == false) {
			sops[i].sem_num = i;
		} else {
			sops[i].sem_num = array[i];
		}

		sops[i].sem_op = -1;
		sops[i].sem_flg = SEM_UNDO;

		if (_is_blocking == false) {
			sops[i].sem_flg |= IPC_NOWAIT;
		}
	}

	struct timespec t;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec += (int64_t)(time_/1000000LL);
	t.tv_nsec += (int64_t)((time_%1000000LL)*1000LL);

	if (semtimedop(_id, sops, _nsem, &t) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore wait failed");
		}
	}
	
	semctl(_id, 0, SETALL, semctl(_id, 0, GETALL, 0));			
}

void SharedSemaphore::Notify(int *array, int array_size, int n)
{
	if (n < 0) {
		throw jexception::OutOfBoundsException("Notify's parameter is out of bounds");
	}

	int sz = _nsem;
	bool param = false;

	if (array != NULL && array_size > 0) {
		sz = array_size;
		param = true;
	}
		
	struct sembuf sops[sz];

	for (int i=0; i<sz; i++) {
		if (param == false) {
			sops[i].sem_num = i;
		} else {
			sops[i].sem_num = array[i];
		}

		sops[i].sem_op = n;
		sops[i].sem_flg = SEM_UNDO;

		if (_is_blocking == false) {
			sops[i].sem_flg |= IPC_NOWAIT;
		}
	}
	
	if (semop(_id, sops, _nsem) != 0) {
		if (errno == EAGAIN) {
			if (_is_blocking == false) {
				throw jexception::SemaphoreException("Shared semaphore no wait exception");
			} else {
				throw jexception::SemaphoreTimeoutException("Shared semaphore timeout exception");
			}
		} else {
			throw jexception::SemaphoreException("Shared semaphore notify failed");
		}
	}
			
	semctl(_id, 0, SETALL, semctl(_id, 0, GETALL, 0));			
}

void SharedSemaphore::NotifyAll(int *array, int array_size)
{
	int count = semctl(_id, 0, GETNCNT, 0);

	if(count > 0) {
		Notify(array, array_size, count);
	}
}

void SharedSemaphore::Release() 
{
	union semun arg;
	
	if (semctl(_id, 0, IPC_RMID, arg) < 0) {
		throw jexception::SemaphoreException("Release shared semaphore error");
	}
}

}

