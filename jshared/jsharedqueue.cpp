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
#include "jsharedqueue.h"
#include "jqueueexception.h"

namespace jshared {

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
	/* union semun is defined by including <sys/sem.h> */
#else
	/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                  /* value for SETVAL */
	struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;    /* array for GETALL, SETALL */
	struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif
	
SharedQueue::SharedQueue(jkey_t key_, int npages_, int struct_size_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedQueue");

#ifdef _WIN32
#else
	int semid;
	struct jshmprefix_t *p;
	union semun arg;
	unsigned short values[1];
	
	// TODO:: verificar se jah existe e depois abrir
	semid = shmget(key_, npages_*getpagesize(), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	
	if(semid < 0) {
		throw QueueException("Cannot create a fifo");
	}
	
	_shm = (struct jshmhandle_t *)malloc(sizeof(struct jshmhandle_t));
	
	_shm->sid = semid;
	_shm->mem = NULL;
	_shm->privsz = struct_size_;
	_shm->sz = npages_*getpagesize();
	
	p = (struct jshmprefix_t *)shmat(semid, 0, 0);
	p->read = p->write = sizeof(struct jshmprefix_t) + struct_size_;
	memset((char *)(p + sizeof(struct jshmprefix_t)), 0, struct_size_);
	
	shmdt(p);
	
	// TODO:: verificar se jah existe e depois abrir
	_shm->semid = semget(key_, 1, (IPC_CREAT | IPC_EXCL) + 0600);
	
	if(_shm->semid < 0) {
		throw QueueException("Failed to create a semaphore");
	}
	
	values[0] = 1;
	arg.array = values;
	
	if (semctl(_shm->semid, 0, SETALL, arg) < 0) {
		throw QueueException("Failed to init a semaphore");
	}
#endif
}

SharedQueue::~SharedQueue()
{
#ifdef _WIN32
#else
	Close();

	if ((void *)_shm != NULL) {
		delete _shm;
	}
#endif
}

/** Private Functions */

int SharedQueue::LLMemFree()
{
#ifdef _WIN32
	return 0;
#else
	struct jshmprefix_t *p;
	int count;
	
  	p = (struct jshmprefix_t *)_shm->mem;
	
	if (p->write >= p->read) {
		count = _shm->sz - p->write;
		count += p->read - sizeof(struct jshmprefix_t) - _shm->privsz;
	} else {
		count = p->read - p->write;
	}
	
	return count;
#endif
}

int SharedQueue::LLMemUsed()
{
#ifdef _WIN32
	return 0;
#else
	struct jshmprefix_t *p= (struct jshmprefix_t *)_shm->mem;
	int count;
	
	if (p->write >= p->read) {
		return p->write - p->read;
	}
	
	count = _shm->sz - sizeof(struct jshmprefix_t) - _shm->privsz - p->read;
	count += p->write;
	
	return count;
#endif
}

int SharedQueue::LLPut(void *data, int sz)
{
#ifdef _WIN32
	return 0;
#else
	struct jshmprefix_t *p = (struct jshmprefix_t *)_shm->mem;
	int copysz;
	
	if (LLMemFree() < sz) {
		return -1;
	}
	
	p->counter++;
	
	copysz = _shm->sz - p->write;
	if (copysz > sz) {
		copysz = sz;
	};
	
	memcpy((char *)_shm->mem + p->write, data, copysz);
	
	p->write += copysz;
	if (p->write == _shm->sz) {
		p->write = _shm->privsz + sizeof(struct jshmprefix_t);
	}
	
	if (copysz < sz) {
		memcpy((char *)_shm->mem + p->write, &((char *)data)[copysz], sz-copysz);
		
		p->write += sz-copysz;
	}
	
	return sz;
#endif
}

int SharedQueue::LLGet(void *data, int sz)
{
#ifdef _WIN32
	return 0;
#else
	struct jshmprefix_t*p;
	int copysz;
	
	p = (struct jshmprefix_t *)_shm->mem;
	p->counter++;
	
	if (sz <= 0) {
		Unlock();
		throw QueueException("Parameter of ll_get failed");
	}
	
	if (LLMemUsed() < sz) {
		return -1;
	}
	
	if (p->write > p->read) {
		copysz=p->write - p->read;
		if (copysz>sz) {
			copysz=sz;
		}
	} else {
		copysz = _shm->sz - p->read;
		if (copysz>sz) {
			copysz=sz;
		}
	}
	
	memcpy(data,(char*)_shm->mem + p->read,copysz);
	
	p->read+=copysz;
	
	if (p->read == _shm->sz) {
		p->read = _shm->privsz + sizeof(struct jshmprefix_t);
	}
	
	if (copysz < sz) {
		memcpy(&((char *)data)[copysz],(char *)_shm->mem + p->read, sz - copysz);
		p->read += sz-copysz;
	}
	
	return sz;
#endif
}

void SharedQueue::LLHRewind()
{
#ifdef _WIN32
#else
  	struct jshmprefix_t *p = (struct jshmprefix_t *)_shm->mem;
	
	p->read -= sizeof(struct jshmbh_t);
	
	if (p->read < (int)(sizeof(struct jshmprefix_t) + _shm->privsz)) {
		p->read = _shm->sz + p->read - sizeof(struct jshmprefix_t) - _shm->privsz;
	}
#endif
}

void SharedQueue::Lock()
{
#ifdef _WIN32
#else
	struct sembuf op[1];
	
	if (!_shm) {
		throw QueueException("Memory handler uninitialized");
	}
	
	if (!_shm->semid) {
		throw QueueException("Semaphore handler uninitialized");
	}
	
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;
	
	if (semop(_shm->semid, op, 1) == -1) {
		throw QueueException("Semaphore lock failed");
	}
#endif
}

void SharedQueue::Unlock()
{
#ifdef _WIN32
#else
	struct sembuf op[1];
	
	if (!_shm) {
		throw QueueException("Memory handler uninitialized");
	}
	
	if(!_shm->semid) {
		throw QueueException("Semaphore handler uninitialized");
	}
	
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = SEM_UNDO;
	
	if (semop(_shm->semid, op, 1) == -1) {
		throw QueueException("Semaphore unlock failed");
	}
#endif
}

/** End */

void SharedQueue::Attach()
{
#ifdef _WIN32
#else
 	if (_shm->mem) {
 		throw QueueException("Attempt to attach already attached");
  	}
   	
	_shm->mem = shmat(_shm->sid, 0, 0);
	
	if (_shm->mem < 0) {
	  	throw QueueException("Attach memory failed");
	}
#endif
}

void SharedQueue::Dealloc()
{
#ifdef _WIN32
#else
  	union semun ignored;
	
	semctl(_shm->semid, 1, IPC_RMID, ignored);
	shmctl(_shm->sid, IPC_RMID, 0);
#endif
}

void SharedQueue::Detach()
{
#ifdef _WIN32
#else
	if (!_shm->mem) {
	   throw QueueException("Attempt to attach already deatached");
	}
	
	if (shmdt(_shm->mem) < 0) {
		if (errno == EACCES) {
	   		throw QueueException("no access permissions for the requested dettach");
		} else {
	   		throw QueueException("Detach exception");
		}
	}

	_shm->mem = NULL;
#endif
}

void SharedQueue::Setpriv(void *priv)
{
#ifdef _WIN32
#else
 	Lock();
 	memcpy((char *)((char *)_shm->mem + sizeof(struct jshmprefix_t)), priv, _shm->privsz);
 	Unlock();
#endif
}

void SharedQueue::Getpriv(void *priv)
{
#ifdef _WIN32
#else
 	Lock();
 	memcpy(priv, (char *)((char *)_shm->mem + sizeof(struct jshmprefix_t)), _shm->privsz);
 	Unlock();
#endif
}

int SharedQueue::Get(void *data,int sz)
{
#ifdef _WIN32
	return 0;
#else
	if ((void *)data == NULL) {
		throw QueueException("Null pointer in data parameter");
	}
	
	if (sz <= 0) {
		throw QueueException("Invalid value for sz parameter");
	}
	
 	struct jshmbh_t h;
	
	Lock();
	
	if (LLMemUsed() == 0) {
		Unlock();
		
		return -1;
	}
	
	if (LLMemUsed() < (int)sizeof(h)) {
		Unlock();
		throw QueueException("Used mem less the header sz");
	}
	
	LLGet(&h, sizeof(h));
	
	if (h.canary != 0xDEADBEEF) {
		throw QueueException("Unexpected error in fifo");
	}
	
	if (LLMemUsed() < h.sz) {
		throw QueueException("Shared memory is corrupted");
	}
	
	if (h.sz > sz) {
		LLHRewind();
		Unlock();
		
		return -2; // TODO:: pode colocar -1 ?
		
	}
	
	LLGet(data, h.sz);
	Unlock();
	
	return h.sz;
#endif
}

int SharedQueue::Put(void *data, int sz)
{
#ifdef _WIN32
	return 0;
#else
	if ((void *)data == NULL) {
		throw QueueException("Null pointer in data parameter");
	}
	
 	struct jshmbh_t h;
	
	Lock();
	
	if (LLMemFree() <= (int)(sz + sizeof(struct jshmbh_t))) {
		Unlock();
		
		return -1;
	}
	
	h.sz = sz;
	h.canary = 0xDEADBEEF;
	
	LLPut(&h, sizeof(h));
	LLPut(data, sz);
	Unlock();

	return sz;
#endif
}

bool SharedQueue::IsEmpty()
{
#ifdef _WIN32
	return true;
#else
 	if ((void *)_shm == NULL) {
		throw QueueException("Parameter shm is null");
	}
	
	struct jshmprefix_t *p = (struct jshmprefix_t *)_shm->mem;
	
	if (p == NULL) {
		return true;
	}
	
	if (p->read == p->write) {
		return true;
	}
	
	return false;
#endif
}

void SharedQueue::Close()
{
}

}