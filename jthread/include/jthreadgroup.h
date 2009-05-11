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
#ifndef J_THREADGROUP_H
#define J_THREADGROUP_H

#include "jrunnable.h"
#include "jthread.h"
#include "jsemaphore.h"
#include "jobject.h"

#include <map>
#include <queue>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jthread{

enum jthread_group_priority_t {
	THREAD_GROUP_LOW,
	THREAD_GROUP_MEDIUM,
	THREAD_GROUP_HIGH
};

class ThreadGroup;

class WorkerThread : public jthread::Thread{

	private:
		/** \brief */
		ThreadGroup *_group;
		/** \brief */
		Runnable *_runnable;
		/** \brief */
		Semaphore _sem;
		/** \brief */
		Mutex _mutex;
		/** \brief */
		bool _is_locked;

	public:
		/**
		 * \brief
		 *
		 */
		WorkerThread(ThreadGroup *group);

		/**
		 * \brief
		 *
		 */
		virtual ~WorkerThread();

		/**
		 * \brief 
		 *
		 */
		bool IsRunning();

		/**
		 * \brief 
		 *
		 */
		void WaitThread();

		/**
		 * \brief 
		 *
		 */
		void AttachThread(Runnable *r);

		/**
		 * \brief 
		 *
		 */
		void DetachThread(Runnable *r);

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

/**
 * \brief ThreadGroup.
 *
 * @author Jeff Ferr
 */
class ThreadGroup : public virtual jcommon::Object{

    private:
		/** \brief */
		std::vector<WorkerThread *> _threads;
		/** \brief */
		std::queue<Runnable *> _low_threads;
		/** \brief */
		std::queue<Runnable *> _medium_threads;
		/** \brief */
		std::queue<Runnable *> _high_threads;
		/** \brief */
		Mutex _mutex;
		/** \brief */
		Semaphore _sem;
		/** \brief */
		int _max_threads;

    public:
		/**
		 * \brief Construtor.
		 *
		 */
		ThreadGroup(int max_threads);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~ThreadGroup();

		/**
		 * \brief 
		 *
		 */
		bool AttachThread(Runnable *r, jthread_group_priority_t t = THREAD_GROUP_MEDIUM);

		/**
		 * \brief 
		 *
		 */
		bool DetachThread(Runnable *r);

		/**
		 * \brief 
		 *
		 */
		void Interrupt();

		/**
		 * \brief
		 *
		 */
		void WaitForAll();

		/**
		 * \brief Return true if thread is started, false if not.
		 *
		 */
		int CountActiveThreads();

		/**
		 * \brief
		 *
		 */
		void ReleaseWorkerThread(WorkerThread *t);

};

}

#endif
