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
#ifndef J_THREAD_H
#define J_THREAD_H

#include "jmutex.h"
#include "jcondition.h"
#include "jthreadexception.h"
#include "jobject.h"
#include "jrunnable.h"

#include <map>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jthread{

enum jthread_type_t {
	DETACH_THREAD,
	JOINABLE_THREAD
};

enum jthread_priority_t {
	LOW_PRIORITY,
	NORMAL_PRIORITY,
	HIGH_PRIORITY
};

enum jthread_policy_t {
	POLICY_OTHER,				// Normal thread
	POLICY_FIFO,				// Real-time thread fifo
	POLICY_ROUND_ROBIN	// Round robin thread round robin
};

#ifdef _WIN32
typedef HANDLE jthread_t;
#else
typedef pthread_t jthread_t;
#endif

struct jthread_map_t {
	jthread_t thread;
	bool alive;
};

class ThreadGroup;

/**
 * \brief Thread.
 *
 * @author Jeff Ferr
 */
class Thread : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		/** \brief */
		DWORD _thread_id;
		/** \brief */
		DWORD _stackSize;
		/** \brief */
		DWORD _executeFlag;
		/** \brief */
		LPTHREAD_START_ROUTINE _threadFunction;
		/** \brief */
		LPVOID _threadArgument;
		/** \brief */
		LPSECURITY_ATTRIBUTES _sa;
		/** \brief */
		DWORD _exitCode;
#else
		/** \brief */
		Condition _condition;
#endif
		/** \brief */
		jthread_t _thread;
		/** \brief */
		std::map<int, jthread_map_t *> _threads;
		/** \brief */
		Mutex jthread_mutex;
		/** \brief */
		Runnable *_runnable;
		/** \brief */
		ThreadGroup *_group;
		/** \brief */
		jthread_type_t _type;
		/** \brief */
		int _id;
		/** \brief */
		bool _is_running;

		/**
		 * \brief
		 *
		 */
#ifdef _WIN32
		static DWORD WINAPI ThreadMain(void *owner_);
#else
		static void * ThreadMain(void *owner_);
#endif

		/**
		 * \brief Notify the end of thread.
		 *
		 */
		jthread_map_t * GetMap(int id);

	protected:
		/**
		 * \brief
		 *
		 */
		virtual int SetUp();

		/**
		 * \brief Dont use try-catch(...)
		 *
		 */
		virtual void Run();

		/**
		 * \brief
		 *
		 */
		virtual int CleanUp();

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		Thread(jthread_type_t type = JOINABLE_THREAD, ThreadGroup *group = NULL);

		/**
		 * \brief Construtor.
		 *
		 */
		Thread(Runnable *runnable, jthread_type_t type = JOINABLE_THREAD, ThreadGroup *group = NULL);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Thread();

		/**
		 * \brief
		 *
		 */
		ThreadGroup * GetThreadGroup();

		/**
		 * \brief
		 *
		 */
		static void Sleep(uint64_t time_);

		/**
		 * \brief
		 *
		 */
		static void MSleep(uint64_t time_);

		/**
		 * \brief
		 *
		 */
		static void USleep(uint64_t time_);

		/**
		 * \brief Get thread identifier
		 *
		 */
		jthread_t GetHandler();

		/**
		 * \brief Get thread identifier
		 *
		 */
		int GetID();

		/**
		 * \brief 
		 *
		 */
		void KillAllThreads();

		/**
		 * \brief Init the thread.
		 *
		 */
		void Start(int id = 0);

		/**
		 * \brief Interrupt the thread.
		 *
		 */
		bool Interrupt(int id = 0);

		/**
		 * \brief Suspend the thread.
		 *
		 */
		void Suspend(int id = 0);

		/**
		 * \brief Resume the thread.
		 *
		 */
		void Resume(int id = 0);

		/**
		 * \brief Return true if thread is started, false if not.
		 *
		 */
		bool IsRunning(int id = 0);

		/**
		 * \bried
		 *
		 */
		void Yield();

		/**
		 * \bried
		 *
		 */
		void SetPolicy(jthread_policy_t policy, jthread_priority_t priority);

		/**
		 * \bried
		 *
		 */
		void GetPolicy(jthread_policy_t *policy, jthread_priority_t *priority);

		/**
		 * \brief Wait for end of thread.
		 *
		 */
		void WaitThread(int id = 0);

		/**
		 * \brief Wait for end of thread.
		 *
		 */
		void Release();

};

}

#endif
