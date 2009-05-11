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

#include <map>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jthread{

struct jthread_map_t {
#ifdef _WIN32
	HANDLE thread;
#else
	pthread_t thread;
#endif
	bool alive;
};

enum jthread_type_t {
	DETACH_THREAD,
	JOINABLE_THREAD
};

enum jthread_policy_t {
#ifdef _WIN32
	POLICY_OTHER		= 1,	// Normal thread
	POLICY_FIFO			= 2,	// Real-time thread fifo
	POLICY_ROUND_ROBIN	= 3		// Round robin thread round robin
#else
	POLICY_OTHER		= SCHED_OTHER,	// Normal thread
	POLICY_FIFO			= SCHED_FIFO,	// Real-time thread fifo
	POLICY_ROUND_ROBIN	= SCHED_RR		// Round robin thread round robin
#endif
};

enum jthread_priority_t {
	LOW_PRIORITY = 0,
	NORMAL_PRIORITY = 5,
	HIGH_PRIORITY = 10
};

/**
 * \brief Thread.
 *
 * @author Jeff Ferr
 */
class Thread : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		/** \brief */
		HANDLE _thread;
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
		pthread_t _thread;
		/** \brief */
		Condition _condition;
#endif
		/** \brief */
		Mutex jthread_mutex;
		/** \brief */
		jthread_type_t _type;
		/** \brief */
		int _key;
		/** \brief */
		bool _is_running;
		/** \brief */
		std::map<int, jthread_map_t *> _threads;

		/**
		 * \brief
		 *
		 */
#ifdef _WIN32
		static DWORD WINAPI ThreadMain(void *owner_);
#else
		static void * ThreadMain(void *owner_);
		static void CleanUpMain(void *owner_);
#endif

		/**
		 * \brief Notify the end of thread.
		 *
		 */
		void SignalThreadDead();

		/**
		 * \brief Notify the end of thread.
		 *
		 */
		jthread_map_t * GetMap(int key);

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
		Thread(jthread_type_t type = JOINABLE_THREAD);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Thread();

		/**
		 * \brief Get thread identifier
		 *
		 */
		int GetId();

		/**
		 * \brief 
		 *
		 */
		void KillAllThreads();

		/**
		 * \brief Init the thread.
		 *
		 */
		void Start(int key = 0);

		/**
		 * \brief Interrupt the thread.
		 *
		 */
		bool Interrupt(int key = 0);

		/**
		 * \brief Suspend the thread.
		 *
		 */
		void Suspend(int key = 0);

		/**
		 * \brief Resume the thread.
		 *
		 */
		void Resume(int key = 0);

		/**
		 * \brief Return true if thread is started, false if not.
		 *
		 */
		bool IsRunning(int key = 0);

		/**
		 * \brief
		 *
		 */
		static void Sleep(long long time_);

		/**
		 * \brief
		 *
		 */
		static void USleep(long long time_);

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
		void WaitThread(int key = 0);

};

}

#endif
