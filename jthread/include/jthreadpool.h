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
#ifndef J_THREADPOOL_H
#define J_THREADPOOL_H

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

enum jthreadpool_priority_t {
	JTP_GROUP_LOW,
	JTP_GROUP_MEDIUM,
	JTP_GROUP_HIGH
};

class ThreadPool;

class WorkerThread : public jthread::Thread{

	private:
		/** \brief */
		ThreadPool *_group;
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
		WorkerThread(ThreadPool *group);

		/**
		 * \brief
		 *
		 */
		virtual ~WorkerThread();

		/**
		 * \brief 
		 *
		 */
		virtual bool IsRunning();

		/**
		 * \brief 
		 *
		 */
		virtual void WaitThread();

		/**
		 * \brief 
		 *
		 */
		virtual void AttachThread(Runnable *r);

		/**
		 * \brief 
		 *
		 */
		virtual void DetachThread(Runnable *r);

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

/**
 * \brief ThreadPool.
 *
 * @author Jeff Ferr
 */
class ThreadPool : public virtual jcommon::Object{

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
		ThreadPool(int max_threads);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~ThreadPool();

		/**
		 * \brief 
		 *
		 */
		virtual bool AttachThread(Runnable *r, jthreadpool_priority_t t = JTP_GROUP_MEDIUM);

		/**
		 * \brief 
		 *
		 */
		virtual bool DetachThread(Runnable *r);

		/**
		 * \brief 
		 *
		 */
		virtual void Interrupt();

		/**
		 * \brief
		 *
		 */
		virtual void WaitForAll();

		/**
		 * \brief Return true if thread is started, false if not.
		 *
		 */
		virtual int CountActiveThreads();

		/**
		 * \brief
		 *
		 */
		virtual void ReleaseWorkerThread(WorkerThread *t);

};

}

#endif
