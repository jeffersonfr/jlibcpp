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
#include "jthreadgroup.h"
#include "jthreadexception.h"
#include "jsemaphoreexception.h"

#include <errno.h>

namespace jthread {

WorkerThread::WorkerThread(ThreadGroup *group)
{
	_group = group;
	_runnable = NULL;
	_is_locked = false;
}

WorkerThread::~WorkerThread()
{
	// TODO:: delete all workthreads
}

bool WorkerThread::IsRunning()
{
	return _is_locked;
}

void WorkerThread::WaitThread()
{
	while (IsRunning() == true) {
		jthread::AutoLock lock(&_mutex);
	}

}

void WorkerThread::AttachThread(Runnable *r)
{
	if ((void *)r == NULL) {
		return;
	}

	_runnable = r;
	_is_locked = true;

	_sem.Notify();
}

void WorkerThread::DetachThread(Runnable *r)
{
	if ((void *)r == NULL) {
		return;
	}

	if (_runnable == r) {
		Interrupt();
		WaitThread();

		_runnable = NULL;
	
		_sem.Notify();
	}
}

void WorkerThread::Run()
{
	do {
		try {
			_sem.Wait();
		
			jthread::AutoLock lock(&_mutex);

			_is_locked = true;

			try {
				_runnable->Routine();
			} catch (...) {
				//  handle any exception
			}

			_is_locked = false;

			_group->ReleaseWorkerThread(this);
		} catch (SemaphoreException &e) {
			if (_is_locked == true) {
				_is_locked = false;

				_group->ReleaseWorkerThread(this);
			}
		}
	} while (true);
}

ThreadGroup::ThreadGroup(int max_threads)
{
	_max_threads = max_threads;

	for (int i=0; i<_max_threads; i++) {
		WorkerThread *t = new WorkerThread(this);

		_threads.push_back(t);

		t->Start();
	}
}

ThreadGroup::~ThreadGroup()
{
	Interrupt();
}

bool ThreadGroup::AttachThread(Runnable *r, jthread_group_priority_t p)
{
	if ((void *)r == NULL) {
		return false;
	}
	
	AutoLock lock(&_mutex);

	for (std::vector<WorkerThread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		if ((*i)->IsRunning() == false) {
			(*i)->AttachThread(r);

			return true;
		}
	}

	if (p == THREAD_GROUP_LOW) {
		_low_threads.push(r);
	} else if (p == THREAD_GROUP_LOW) {
		_medium_threads.push(r);
	} else if (p == THREAD_GROUP_LOW) {
		_high_threads.push(r);
	}

	return false;
}

bool ThreadGroup::DetachThread(Runnable *r)
{
	if ((void *)r == NULL) {
		return false;
	}
	
	AutoLock lock(&_mutex);

	for (std::vector<WorkerThread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		if ((*i)->IsRunning() == true) {
			(*i)->DetachThread(r);

			break;
		}
	}

	for (std::vector<WorkerThread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		if ((*i)->IsRunning() == true) {
			(*i)->DetachThread(r);

			break;
		}
	}

	// TODO:: remove runnable
	/*
	if (p == THREAD_GROUP_LOW) {
		_low_threads.push(r);
	} else if (p == THREAD_GROUP_LOW) {
		_medium_threads.push(r);
	} else if (p == THREAD_GROUP_LOW) {
		_high_threads.push(r);
	}
	*/

	return true;
}

void ThreadGroup::Interrupt()
{
	AutoLock lock(&_mutex);

	for (std::vector<WorkerThread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		try {
			if ((*i)->IsRunning() == false) {
				(*i)->Interrupt();
			}
		} catch (...) {
		}
	}
}

void ThreadGroup::WaitForAll()
{
	do {
		_sem.Wait();
	// } while (CountActiveThreads() > 0);
	} while (CountActiveThreads() > 0 || _low_threads.size() > 0 || _medium_threads.size() > 0 || _high_threads.size() > 0);
}

int ThreadGroup::CountActiveThreads()
{
	AutoLock lock(&_mutex);

	int count = 0;

	for (std::vector<WorkerThread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		if ((*i)->IsRunning() == true) {
			count++;
		}
	}

	return count;
}

void ThreadGroup::ReleaseWorkerThread(WorkerThread *t)
{
	if ((void *)t == NULL) {
		return;
	}

	AutoLock lock(&_mutex);

	Runnable *r = NULL;

	if (_high_threads.size() > 0) {
		r = _high_threads.front();
		_high_threads.pop();
	} else if (_medium_threads.size() > 0) {
		r = _medium_threads.front();
		_medium_threads.pop();
	} else if (_low_threads.size() > 0) {
		r = _low_threads.front();
		_low_threads.pop();
	}

	if ((void *)r != NULL) {
		t->AttachThread(r);
	}

	_sem.Notify();

}

}

