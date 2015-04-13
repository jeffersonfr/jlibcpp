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
#include "jsemaphore.h"
#include "jrunnable.h"
#include "jthreadexception.h"

#include <map>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jthread{

enum jthread_type_t {
	JTT_DETACH,
	JTT_JOINABLE
};

enum jthread_cancel_t {
	JTC_DISABLED,
	JTC_DEFERRED,
	JTC_ASYNCHRONOUS
};

enum jthread_priority_t {
	JTP_LOW,
	JTP_NORMAL,
	JTP_HIGH
};

enum jthread_policy_t {
	JTP_OTHER,			// Normal thread
	JTP_FIFO,				// Real-time thread fifo
	JTP_ROUND_ROBIN	// Round robin thread round robin
};

#ifdef _WIN32
typedef HANDLE jthread_t;
#else
typedef pthread_t jthread_t;
#endif

class Thread;
class ThreadGroup;

struct jthread_map_t {
	Thread *thiz;
	jthread_t thread;
	bool detached;
	bool alive;
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
#endif
		/** \brief */
		std::map<int, jthread_map_t *> _threads;
		/** \brief */
		Mutex _thread_mutex;
		/** \brief */
		Semaphore _thread_semaphore;
		/** \brief */
		Runnable *_runnable;
		/** \brief */
		ThreadGroup *_group;
		/** \brief */
		jthread_cancel_t _cancel;
		/** \brief */
		jthread_type_t _type;
		/** \brief */
		size_t _stack_size;
		/** \brief */
		int _id;

#ifdef _WIN32
		/**
		 * \brief
		 *
		 */
		static DWORD WINAPI ThreadMain(void *owner_);
		
		/**
		 * \brief
		 *
		 */
		static void WINAPI ThreadCleanup(void *owner_);
#else
		/**
		 * \brief
		 *
		 */
		static void * ThreadMain(void *owner_);
		
		/**
		 * \brief
		 *
		 */
		static void ThreadCleanup(void *owner_);
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
		virtual void Setup();

		/**
		 * \brief Dont use try-catch (...)
		 *
		 */
		virtual void Run();

		/**
		 * \brief
		 *
		 */
		virtual void Cleanup();

		/**
		 * \brief
		 *
		 */
		virtual void CancelHook();

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		Thread(jthread_type_t type = JTT_JOINABLE, ThreadGroup *group = NULL);

		/**
		 * \brief Construtor.
		 *
		 */
		Thread(Runnable *runnable, jthread_type_t type = JTT_JOINABLE, ThreadGroup *group = NULL);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Thread();

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
		 * \brief
		 *
		 */
		virtual bool IsJoinable();

		/**
		 * \brief
		 *
		 */
		virtual void SetCancelType(jthread_cancel_t type);

		/**
		 * \brief
		 *
		 */
		virtual jthread_cancel_t GetCancelType();

		/**
		 * \brief
		 *
		 */
		virtual ThreadGroup * GetThreadGroup();

		/**
		 * \bried
		 *
		 */
		virtual void YieldThread();

		/**
		 * \brief Get thread identifier
		 *
		 */
		virtual jthread_t GetHandler(int id = 0);

		/**
		 * \brief Get thread identifier
		 *
		 */
		virtual int GetID();

		/**
		 * \brief 
		 *
		 */
		virtual void SetStackSize(int size);

		/**
		 * \brief 
		 *
		 */
		virtual int GetStackSize();

		/**
		 * \brief Init the thread.
		 *
		 */
		virtual void Start(int id = 0);

		/**
		 * \brief Interrupt the thread.
		 *
		 */
		virtual bool Interrupt(int id = 0);

		/**
		 * \brief Suspend the thread.
		 *
		 */
		virtual void Suspend(int id = 0);

		/**
		 * \brief Resume the thread.
		 *
		 */
		virtual void Resume(int id = 0);

		/**
		 * \brief Return true if thread is started, false if not.
		 *
		 */
		virtual bool IsRunning(int id = 0);

		/**
		 * \bried
		 *
		 */
		virtual void SetPolicy(int id, jthread_policy_t policy, jthread_priority_t priority);

		/**
		 * \bried
		 *
		 */
		virtual void GetPolicy(int id, jthread_policy_t *policy, jthread_priority_t *priority);

		/**
		 * \brief Wait for end of thread.
		 *
		 */
		virtual void WaitThread(int id = 0);

		/**
		 * \brief Wait for end of thread.
		 *
		 */
		virtual void Release();

};

}

#endif
