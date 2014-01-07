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
#include "jthread.h"
#include "jthreadgroup.h"
#include "jthreadexception.h"
#include "jautolock.h"

namespace jthread {

Thread::Thread(jthread_type_t type, ThreadGroup *group):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Thread");

	_runnable = NULL;
	_type = type;
	_group = group;
	_stack_size = -1;

#ifdef _WIN32
	_sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
	_sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	_sa->lpSecurityDescriptor = NULL;
	_sa->bInheritHandle = FALSE;
#else
#endif
}

Thread::Thread(Runnable *runnable, jthread_type_t type, ThreadGroup *group):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::Thread");

	_runnable = runnable;
	_type = type;
	_group = group;

#ifdef _WIN32
	_sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
	_sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	_sa->lpSecurityDescriptor = NULL;
	_sa->bInheritHandle = FALSE;
#else
#endif

	if (_group != NULL) {
		_group->RegisterThread(this);
	}
}

Thread::~Thread()
{
	Release();
}

ThreadGroup * Thread::GetThreadGroup()
{
	return _group;
}

void Thread::Sleep(uint64_t time_)
{
#ifdef _WIN32
	::Sleep((DWORD)(time_*1000LL));
#else
	// usleep((useconds_t)(time_*1000000LL));
	sleep((useconds_t)time_);
#endif
}

void Thread::MSleep(uint64_t time_)
{
#ifdef _WIN32
	::Sleep((DWORD)time_);
#else
	usleep((useconds_t)(time_*1000LL));
#endif
}

void Thread::USleep(uint64_t time_)
{
#ifdef _WIN32
	::Sleep((DWORD)(time_/999LL));
#else
	usleep((useconds_t)time_);
#endif
}

void Thread::Yield()
{
#ifdef _WIN32
	sched_yield();
#else
	pthread_yield();
#endif
}

jthread_t Thread::GetHandler(int id)
{
	for (std::map<int, jthread_map_t *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		jthread_map_t *t = i->second;

		if (i->first == id) {
			return t->thread;
		}
	}

	return 0;
}

int Thread::GetID()
{
	for (std::map<int, jthread_map_t *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		jthread_map_t *t = i->second;

#ifdef _WIN32
		if (t->thread == GetCurrentThread()) {
			return i->first;
		}
#else
		if (t->thread == pthread_self()) {
			return i->first;
		}
#endif
	}

	return -1;
}

void Thread::SetStackSize(int size)
{
	_stack_size = size;
}

/** Private */

#ifdef _WIN32
DWORD WINAPI Thread::ThreadMain(LPVOID owner_)
#else
void * Thread::ThreadMain(void *owner_)
#endif
{
	jthread_map_t *t = (jthread_map_t *)owner_;

#ifdef _WIN32
#else
	if (t->thiz->_type == JTT_JOINABLE) {
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	} else if (t->thiz->_type == JTT_DETACH) {
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	}

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif

	t->thiz->Run();

	t->alive = false;

	pthread_exit(NULL);
}

jthread_map_t * Thread::GetMap(int id)
{
	for (std::map<int, jthread_map_t *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		if (id == i->first) {
			return i->second;
		}
	}

	return NULL;
}

/** End */

/** Protected */

void Thread::Setup()
{
}

void Thread::Run() 
{
	if (_runnable != NULL) {
		_runnable->Run();
	}

	return;
}

void Thread::Cleanup()
{
}

/** End */

void Thread::Start(int id)
{
	AutoLock lock(&jthread_mutex);

	Setup();

	jthread_map_t *t = GetMap(id);

	if ((void *)t == NULL) {
		t = new jthread_map_t;
	
		_threads[id] = t;
	}

	t->thiz = this;
	t->thread = 0;
	t->detached = false;
	t->alive = true;

#ifdef _WIN32
	if (_thread = CreateThread(
				_sa,				// security attributes
				0,					// stack size
				Thread::ThreadMain,	// function thread
				t,					// thread arguments
				0,					// flag
				&_thread_id) == NULL) {
		throw ThreadException("Create thread failed");
	}

	t->thread = _thread;
#else
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	if (_type == JTT_DETACH) {
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	} else {
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	}

	if (_stack_size > 0) {
		if (pthread_attr_setstacksize(&attr, _stack_size) != 0) {
			throw ThreadException("Thread stack exception");
		}
	}
			
	// pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	if (pthread_create(&_thread, NULL, &(Thread::ThreadMain), t)) {
		t->detached = true;
		t->alive = false;

		pthread_attr_destroy(&attr);

		throw ThreadException("Thread create failed");
	}

	t->thread = _thread;

	pthread_attr_destroy(&attr);
#endif
}

bool Thread::Interrupt(int id)
{
	AutoLock lock(&jthread_mutex);

	jthread_map_t *t = GetMap(id);

	if ((void *)t == NULL || t->alive == false) {
		return true;
	}

	t->alive = false;

#ifdef _WIN32
	if (TerminateThread(_thread, 0) == FALSE) {
		return false; // CHANGE:: throw ThreadException("Thread cancel exception");
	}

	/*
	if ((void *)_sa != NULL) {
		 HeapFree(GetProcessHeap(), 0, _sa); 

		 _sa = NULL;
	}
	*/

	/* CHANGE::
	if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED) {
		 return false; // CHANGE:: throw ThreadException("Wait thread failed");
	}
	*/
#else
	if (_type == JTT_JOINABLE) {
		pthread_cancel(t->thread);
	} else if (_type == JTT_DETACH) {
		pthread_cancel(t->thread);
	}
#endif

	return true;
}

void Thread::Suspend(int id) 
{
	AutoLock lock(&jthread_mutex);

	jthread_map_t *t = GetMap(id);

	if ((void *)t == NULL || t->alive == false) {
		return;
	}

	t->alive = false;

#ifdef _WIN32
	HANDLE thread = t->thread;

	if (SuspendThread(thread) == 0xFFFFFFFF) {
		throw ThreadException("Suspend thead failed");
	}
#else
	Interrupt(id);
#endif
}

void Thread::Resume(int id)
{
	AutoLock lock(&jthread_mutex);

	jthread_map_t *t = GetMap(id);

	if ((void *)t == NULL || t->alive == false) {
		return;
	}

#ifdef _WIN32

	if (ResumeThread(t->thread) == 0xFFFFFFFF) {
		throw ThreadException("Resume thread failed");
	}

	t->alive = true;
#else
	throw ThreadException("Thread resume not implemented");
#endif
}

bool Thread::IsRunning(int id)
{
	AutoLock lock(&jthread_mutex);

	jthread_map_t *t = GetMap(id);

	if ((void *)t != NULL) {
		return t->alive;
	}

	return false;
}

void Thread::SetPolicy(jthread_policy_t policy, jthread_priority_t priority)
{
	AutoLock lock(&jthread_mutex);

	int tpriority = 0;

#ifdef _WIN32
	if (priority == JTP_LOW) {
		tpriority = THREAD_PRIORITY_BELOW_NORMAL;
	} else if (priority == JTP_NORMAL) {
		tpriority = THREAD_PRIORITY_NORMAL;
	} else if (priority == JTP_HIGH) {
		tpriority = THREAD_PRIORITY_ABOVE_NORMAL;
	}
#else
	if (priority == JTP_LOW) {
		tpriority = 0;
	} else if (priority == JTP_NORMAL) {
		tpriority = 5;
	} else if (priority == JTP_HIGH) {
		tpriority = 10;
	}
#endif

	int tpolicy = 0;

#ifdef _WIN32
	if (policy == JTP_OTHER) {
		tpolicy = 1;
	} else if (policy == JTP_FIFO) {
		tpolicy = 2;
	} else if (policy == JTP_ROUND_ROBIN) {
		tpolicy = 3;
	}
#else
	if (policy == JTP_OTHER) {
		tpolicy = SCHED_OTHER;
	} else if (policy == JTP_FIFO) {
		tpolicy = SCHED_FIFO;
	} else if (policy == JTP_ROUND_ROBIN) {
		tpolicy = SCHED_RR;
	}
#endif

#ifdef _WIN32
	if (SetThreadPriority(_thread, tpriority) == 0) {
		throw ThreadException("Unknown exception in set policy");
	}
#else
	struct sched_param param;

	// param.sched_priority = tpriority;
	param.__sched_priority = tpriority;

	int result;

	result = pthread_setschedparam(_thread, tpolicy, &param);

	if (result == EINVAL) {
		throw ThreadException("Policy is not defined");
	} else if (result == EPERM) {
		throw ThreadException("The process does not have superuser permission");
	} else if (result == ESRCH) {
		throw ThreadException("This thread has already terminated");
	} else if (result < 0) {
		throw ThreadException("Unknown exception in set policy");
	}
#endif
}

void Thread::GetPolicy(jthread_policy_t *policy, jthread_priority_t *priority) 
{
	AutoLock lock(&jthread_mutex);

#ifdef _WIN32
	int r;

	if ((r = GetThreadPriority(_thread)) == THREAD_PRIORITY_ERROR_RETURN) {
		throw ThreadException("Unknown exception in get policy");
	}

	switch (r) {
		case 0:
			(*priority) = JTP_LOW;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			(*priority) = JTP_NORMAL;
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			(*priority) = JTP_HIGH;
			break;
		default:
			(*priority) = JTP_NORMAL;
			break;
	}

	(*policy) = JTP_OTHER;
#else
	struct sched_param param;

	int policy_int, result;

	result = pthread_getschedparam(_thread, &policy_int, &param);

	if (result == ESRCH) {
		throw ThreadException("This thread has already terminated");
	} else if (result < 0) {
		throw ThreadException("Unknown exception in get policy");
	}

	// switch (param.sched_priority) {
	switch (param.__sched_priority) {
		case 0:
			(*priority) = JTP_LOW;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			(*priority) = JTP_NORMAL;
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			(*priority) = JTP_HIGH;
			break;
		default:
			(*priority) = JTP_NORMAL;
			break;
	}

	switch (policy_int) {
		case SCHED_OTHER:
			(*policy) = JTP_OTHER;
			break;
		case SCHED_FIFO:
			(*policy) = JTP_FIFO;
			break;
		case SCHED_RR:
			(*policy) = JTP_ROUND_ROBIN;
			break;
	}
#endif
}

void Thread::WaitThread(int id)
{
	AutoLock lock(&jthread_mutex);

	jthread_map_t *t = GetMap(id);

	if ((void *)t == NULL || t->alive == false) {
		return;
	}

#ifdef _WIN32
	if (WaitForSingleObject(t->thread, INFINITE) == WAIT_FAILED) {
		throw ThreadException("Wait thread failed");
	}
#else
	if (_type == JTT_JOINABLE) {
		if (t->detached == false) {
			jthread_mutex.Unlock();
			pthread_join(t->thread, NULL);
			jthread_mutex.Lock();
			pthread_detach(t->thread);
	
			Cleanup();

			t->detached = true;
			t->alive = false;
		}
	}
#endif
}

void Thread::Release()
{
	AutoLock lock(&jthread_mutex);

	if (_group != NULL) {
		_group->UnregisterThread(this);
	}

	for (std::map<int, jthread_map_t *>::iterator i=_threads.begin(); i != _threads.end(); i++) {
		delete i->second;
	}

	_threads.clear();

#ifdef _WIN32
	if ((void *)_sa != NULL) {
		HeapFree(GetProcessHeap(), 0, _sa); 

		_sa = NULL;
	}
#endif
}

}
