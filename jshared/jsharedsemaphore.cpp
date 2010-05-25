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
#include "jsharedlib.h"
#include "jthreadlib.h"

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
	
#ifdef _WIN32
SharedSemaphore::SharedSemaphore(int key_):
#else 
SharedSemaphore::SharedSemaphore(key_t key_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedSemaphore");
	
#ifdef _WIN32
#else	
	_id = semget(key_, 0, 0);

	if (_id < 0) {
		throw jthread::SemaphoreException("Opening shared semaphore error");
	}
#endif
}

#ifdef _WIN32
SharedSemaphore::SharedSemaphore(int key_, int nsem_, int value_, int perms_):
#else 
SharedSemaphore::SharedSemaphore(key_t key_, int nsem_, int value_, int perms_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::SharedSemaphore");

#ifdef _WIN32
#else	
	_nsem = nsem_;
	_value = value_;
	_flag = 0;
	
	_id = semget(key_, nsem_, perms_ | IPC_CREAT | IPC_EXCL);

	if (_id < 0) {
		/*
		if (errno == EEXIST) {
			throw jthread::SemaphoreException("Opening shared semaphore error");
		}
		
		throw jthread::SemaphoreException("Creating shared semaphore error");
		*/
	
		_id = semget(key_, nsem_, perms_);
	
		if (_id < 0) {
			throw jthread::SemaphoreException("Creating shared semaphore error");
		}
	}

	InitializeSemaphore();
#endif
}

void SharedSemaphore::InitializeSemaphore() 
{
#ifdef _WIN32
#else	
	uint16_t *initv = new uint16_t[_nsem];

	for (int i=0; i<_nsem; i++) {
		initv[i] = _value;
	}
	
	union semun arg;

	arg.array = initv;

	int r;

	r = semctl(_id, _nsem, SETALL, arg);

	delete [] initv;
	
	if (r < 0) {
		throw jthread::SemaphoreException("Initializing semaphore error");
	}
#endif
}

SharedSemaphore::~SharedSemaphore()
{
}

void SharedSemaphore::SetBlocking(bool b)
{
#ifdef _WIN32
#else	
	if (b == true) {
		_flag = 0;
	} else {
		_flag = IPC_NOWAIT;
	}
#endif
}

bool SharedSemaphore::IsBlocking()
{
	return (_flag == 0);
}

void SharedSemaphore::SetTimeout(int millis_, jsem_op_t *op)
{
	if (millis_ <= 0) {
		return;
	}

#ifdef _WIN32
#elif __CYGWIN32__
#else	
	struct sembuf *sops;
	jsem_op_t *p = op;
	
	p->id = new int[_nsem];
	
	if (p == NULL) {
		sops = new struct sembuf[_nsem];
		p = new jsem_op_t;
		
		int k;
		for (k=0; k<_nsem; k++) {
			p->id[k] = k;
		}

		p->length = k;
	} else {
		sops = new struct sembuf[p->length];
	}
	
	for (int i=0; i<op->length; i++) {
		if (p->id[i] < _nsem) {
			if (op == NULL) {
				delete p;
			}
			delete sops;
			
			throw jthread::SemaphoreException("Invalid semaphore id error");
		}
		
		sops[i].sem_num = p->id[i];
		sops[i].sem_op = +1;
		sops[i].sem_flg = _flag;
	}
	
	struct timespec t;

	t.tv_sec = millis_/1000;
	t.tv_nsec = 0;

	int r = semtimedop(_id, sops, p->length, &t);

	if (op == NULL) {
		delete p;
	}
	delete sops;
			
	if (r < 0) {
		throw jthread::SemaphoreException("Setting timeout exception");
	}
#endif
}

void SharedSemaphore::Wait(jsem_op_t *op)
{
#ifdef _WIN32
#else	
	// SEE:: semop, SEM_UNDO
	
	struct sembuf *sops;
	jsem_op_t *p = op;
	
	p->id = new int[_nsem];
	
	if (p == NULL) {
		sops = new struct sembuf[_nsem];
		p = new jsem_op_t;

		int k;
		for (k=0; k<_nsem; k++) {
			p->id[k] = k;
		}

		p->length = k;
	} else {
		sops = new struct sembuf[p->length];
	}
	
	for (int i=0; i<op->length; i++) {
		if (p->id[i] < _nsem) {
			if (op == NULL) {
				delete p;
			}
			delete sops;
			
			throw jthread::SemaphoreException("Invalid semaphore id error");
		}
		
		sops[i].sem_num = p->id[i];
		sops[i].sem_op = -1;
		sops[i].sem_flg = _flag;
	}
	
	int r = semop(_id, sops, p->length);

	if (op == NULL) {
		delete p;
	}
	delete sops;
			
	if (r < 0) {
		if (errno == EAGAIN) {
			if (_flag == IPC_NOWAIT) {
				throw jthread::SemaphoreException("Initializing semaphore error");
			} else {
				throw jthread::SemaphoreException("Timeout expired exception");
			}
		} else {
				throw jthread::SemaphoreException("Waiting semaphore error");
		}
	}
#endif
}

void SharedSemaphore::Notify(jsem_op_t *op)
{
#ifdef _WIN32
#else	
	// SEE:: semop, SEM_UNDO
	
	struct sembuf *sops;
	jsem_op_t *p = op;
	
	p->id = new int[_nsem];
	
	if (p == NULL) {
		sops = new struct sembuf[_nsem];
		p = new jsem_op_t;
		
		int k;
		for (k=0; k<_nsem; k++) {
			p->id[k] = k;
		}

		p->length = k;
	} else {
		sops = new struct sembuf[p->length];
	}
	
	for (int i=0; i<op->length; i++) {
		if (p->id[i] < _nsem) {
			if (op == NULL) {
				delete p;
			}
			delete sops;
			
			throw jthread::SemaphoreException("Invalid semaphore id error");
		}
		
		sops[i].sem_num = p->id[i];
		sops[i].sem_op = +1;
		sops[i].sem_flg = _flag;
	}
	
	int r = semop(_id, sops, p->length);

	if (op == NULL) {
		delete p;
	}
	delete sops;
			
	if (r < 0) {
		if (errno == EAGAIN) {
			if (_flag == IPC_NOWAIT) {
				throw jthread::SemaphoreException("Initializing semaphore error");
			} else {
				throw jthread::SemaphoreException("Timeout expired exception");
			}
		} else {
				throw jthread::SemaphoreException("Waiting semaphore error");
		}
	}
#endif
}

void SharedSemaphore::Release() 
{
#ifdef _WIN32
#else	
	union semun arg;
	int r;
	
	r = semctl(_id, 0, IPC_RMID, arg);

	if (r < 0) {
		throw jthread::SemaphoreException("Release semaphores error");
	}
#endif
}

}
