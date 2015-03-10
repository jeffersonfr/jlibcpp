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
#ifndef J_SYNCTHREAD_H
#define J_SYNCTHREAD_H

#include "jmutex.h"
#include "jsemaphore.h"
#include "jrunnable.h"
#include "jthread.h"
#include "jcondition.h"
#include "jthreadexception.h"

#include <string>
#include <list>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jthread{

/** 
 * \brief possible transitions of thread state
 * +------+----------+----------+----------+----------+
 * |      |JSS_INIT      |running   |paused    |completed |
 * +----- +----------+----------+----------+----------+
 * |stop  |completed |completed |completed |completed |
 * |      |true      |true      |true      |true      |
 * +------+----------+----------+----------+----------+
 * |pause |JSS_INIT      |paused    |paused    |completed |
 * |      |false     |true      |true      |false     |
 * +------+----------+----------+----------+----------+
 * |resume|JSS_INIT      |running   |running   |completed |
 * |      |false     |true      |true      |false     |
 * +-----------------+----------+----------+----------+
 * |launch |running   |running   |paused    |completed |
 * |      |true      |true      |false     |false     |
 * +------+----------+----------+----------+----------+
 */
enum jsyncthread_state_t {
	JSS_INIT, // thread did not launch yet
	JSS_PAUSED, // thread is paused
	JSS_RUNNING, // thread is running
	JSS_COMPLETED // thread function is completed
};
    
enum jsyncthread_request_t {
	JSR_NONE, 
	JSR_PAUSE, 
	JSR_STOP
};

class jsyncthread_status_t {

	public:
		bool success;
		bool wait;

	public:
		jsyncthread_status_t(): 
			success(false), wait(false)
		{
		}

		jsyncthread_status_t(bool rc): 
			success(rc), wait(rc)
		{
		}

		jsyncthread_status_t(bool success_, bool wait_): 
			success(success_), wait(wait_)
		{
		}

};

/**
 * \brief Thread wrapper with safe launch/stop/pause/resume methods.
 *
 * \details The class provide safe methods for thread control:
 * <code>launch()</code>, <code>stop()</code>, <code>pause()</code> and <code>resume()</code>.
 *
 * The methods are blocking: they will return only after the operation is completed. The class is abstract. 
 * To use it a derived class must implement pure virtual method <code>action()</code>. The <code>action()</code> 
 * must exit if the <code>is_interrupted()</code> flag is set to <code>true</code>. That requires periodical 
 * flag checking in the <code>action()</code> implemenation code. The checking period defines the waiting time 
 * of the <code>stop()</code>, <code>pause()</code> operations. Also the derived class can implement the 
 * <code>on_interrupt()</code> virtual method. This callback is invoked each time either the <code>stop()</code> 
 * or the <code>pause()</code> method is called. The callback implementation should be used to force the 
 * <code>action()</code> method to check the <code>is_interrupted()</code> flag. For example:
 *
 * <code>
 * virtual bool action() {
 * 		boost::unique_lock<boost::mutex> lock(m_guard);
 * 		while(!is_interrupted()) {
 * 			m_signal.wait(lock);
 * 		}
 * 
 * 		return true;
 * }
 * 
 * virtual void on_interrupt() {
 * 		boost::unique_lock<boost::mutex> lock(m_guard);
 * 
 * 		// in this point is_interrupted() is true
 * 		assert(is_interrupted());
 * 
 * 		m_signal.notify_one();
 * }
 * </code>
 *
 * The <code>stop()</code> (but not <code>pause()</code>) method can use <code>force_interrupt</code> flag: 
 * <code>stop(true)</code>. This flag will interrupt the <code>action()</code> waiting at any <i>boost thread 
 * interruption point</i> (for example, <code>boost::condition_variable::wait()</code>). The <code>action()</code> 
 * will be called again if it returns true. If it returns <code>false</code> the thread execution will be completed.
 *
 * In the paused state the thread will sleep and will not call <code>action()</code> till the execution is resumed. 
 * This paused state can be used for re-configuration of the working thread data. Without such paused state the
 * <code>action()</code> must use locks to protect sensitive data from simultaneous accees by multiple threads. This 
 * approach allows the thread to avoid using locks in run time. The class's virtual <i>dtor</i> will assert if 
 * thread state is not <code>completed</code>. To prevent the assertion <i>dtor</i> of the derived class should call 
 * the <code>stop()</code> method. The derived class also can implement optional virtual methods 
 * <code>on_launch()</code> and <code>on_exit()</code>.
 *
 * An auxiliary <code>BulkSyncThread</code> class performs the blocking <code>launch()</code>, <code>stop()</code>, 
 * <code>pause()</code> and <code>resume()</code> operations simultaneously for several thread objects.
 *
 * @author Jeff Ferr
 */
class SyncThread : public virtual jthread::Thread{

	friend class BulkSyncThread;

	private:
		/** \brief */
		jthread::Mutex m_guard;
		/** \brief */
		jthread::Condition m_signal;
		/** \brief */
		jthread::Condition m_pause;
		/** \brief */
		jthread::Condition m_interrupt;
		/** \brief */
		volatile jsyncthread_state_t m_state;
		/** \brief */
		volatile jsyncthread_request_t m_request;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void Idle();

		/**
		 * \brief
		 *
		 */
		virtual bool Launch();

		/**
		 * \brief
		 *
		 */
		virtual void SignalState(jsyncthread_state_t state);

		/**
		 * \brief
		 *
		 */
		virtual void Request(jsyncthread_request_t rq);

		/**
		 * \brief
		 *
		 */
		virtual void StopRequest(bool force);

		/**
		 * \brief
		 *
		 */
		virtual void WakeUp(jsyncthread_request_t rq);

		/**
		 * \brief
		 *
		 */
		virtual bool WaitTillLaunched();

		/**
		 * \brief
		 *
		 */
		virtual bool WaitTillResumed();

		/**
		 * \brief
		 *
		 */
		virtual bool WaitTillPaused();

		/**
		 * \brief
		 *
		 */
		virtual jsyncthread_status_t StartEvent();

		/**
		 * \brief
		 *
		 */
		virtual jsyncthread_status_t PauseEvent();

		/**
		 * \brief
		 *
		 */
		virtual jsyncthread_status_t ResumeEvent();

		/**
		 * \brief
		 *
		 */
		virtual void Run();

	protected:
		/**
		 * \brief The action callback implemented in the derived class. The callback will be called in the 
		 * context of the running thread again and again till it returns true. To terminate the thread the 
		 * callback should return false. The callback <b>must</b> exit if the <code>is_interrupted()</code> 
		 * flag is set to <code>true</code>. The method is pure virtual and must be implemented in derived 
		 * classes.
		 *
		 * \return code>true</code> to continue the execution and code>false</code> to complete the thread 
		 * function.
		 */
		virtual bool Action();

		/**
		 * \brief The callback is called after the thread launched its execution. The thread state is 
		 * <code>running</code>. The method is called in context of the running thread. The method can be 
		 * implemented in derived classes.
		 */
		virtual void OnStart();

		/**
		 * \brief The callback is called after the thread ceased to call the <code>action</code> method but 
		 * before the thread function is completed. The method is called in context of the running thread.
		 * The thread state is <code>completed</code>. The method can be implemented in derived classes.
		 */
		virtual void OnExit();

		/**
		 * \brief The callback is invoked each time either the <code>stop()</code> or <code>pause()</code> 
		 * is called. A derived class can use this callback to force the <code>action()</code> to check 
		 * the <code>is_interrupted()</code> flag. 
		 */
		virtual void OnInterrupt();

		/**
		 * \brief Signals to the <code>action()</code> to exit. The flag is set on either stop or pause 
		 * request.
		 *
		 */
		virtual bool IsInterrupted();

	public:
		/**
		 * \brief Accepts name and stack size.
		 *
		 */
		SyncThread();

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~SyncThread()
		{
			// stop() must be called before the derived class' dtor is completed. The method stop() cannot 
			// be called here because the thread's main function calls to class's virtual functions including 
			// the pure virtual action().
		}

		/**
		 * \brief Starts the thread. The method is blocking.
		 *
		 */
		virtual bool Start();

		/**
		 * \brief Stops the thread. The method is blocking.
		 *
		 * \param force_interrupt forces using the boost's inerruption points:
		 * 		if the thread in waitng at any of the boost's inerruption points, itwill be interrupted as well.
		 */
		virtual void Stop(bool force = false);

		/**
		 * \brief Pauses the thread. The method is blocking.
		 *
		 */
		virtual bool Pause();

		/**
		 * \brief Resumes the thread. The method is blocking.
		 *
		 */
		virtual bool Resume();

		/**
		 * \brief Gets the thread current state.
		 *
		 */
		virtual jsyncthread_state_t State();

};

/**
 * \brief The BulkSyncThread is an auxiliary class that performs the blocking <code>launch()</code>, 
 * <code>stop()</code>, <code>pause()</code> and <code>resume()</code> operations simultaneously for 
 * several thread objects. 
 *
 * The class accesses the hidden non-blocking methods of the <code>SyncThread</code> class and 
 * performs the <code>launch()</code>, <code>stop()</code>, <code>pause()</code> and <code>resume()</code> 
 * operations simultaneously for several thread objects. The class methods will be completed when the 
 * corresponding operation is completed for every thread object in the BulkSyncThread is complited. This 
 * approach can significantly reduce operation time for a thread pool.
 */
struct BulkSyncThread : public jcommon::Object {

	private:
		std::list<SyncThread *> _objects;

	private:
		static SyncThread* get_ptr(SyncThread *obj)
		{
			return obj;
		}

		static SyncThread* get_ptr(SyncThread &obj)
		{
			return &obj;
		}

		static void LaunchItem(SyncThread *obj, std::list<SyncThread *> &ls)
		{
			jsyncthread_status_t rc = obj->StartEvent();

			if (rc.wait) {
				ls.push_back(obj);
			}
		}

		static void PauseItem(SyncThread *obj, std::list<SyncThread *> &ls)
		{
			jsyncthread_status_t rc = obj->PauseEvent();

			if (rc.wait) {
				ls.push_back(obj);
			}
		}

		static void ResumeItem(SyncThread *obj, std::list<SyncThread *> &ls)
		{
			jsyncthread_status_t rc = obj->ResumeEvent();

			if (rc.wait) {
				ls.push_back(obj);
			}
		}

	public:
		/**
		 * \brief Constructor
		 *
		 */
		BulkSyncThread()
		{
		}

		/**
		 * \brief Constructor that accepts one thread object and adds it to the inner BulkSyncThread list.
		 *
		 */
		BulkSyncThread(SyncThread *obj)
		{
			_objects.push_back(obj);
		}

		/**
		 * \brief Constructor that accepts two thread objects and adds them to the inner BulkSyncThread list.
		 *
		 */
		BulkSyncThread(SyncThread *obj1, SyncThread *obj2)
		{
			_objects.push_back(obj1);
			_objects.push_back(obj2);
		}

		/**
		 * \brief Constructor that accepts three thread objects and adds them to the inner BulkSyncThread list.
		 * 
		 */
		BulkSyncThread(SyncThread *obj1, SyncThread *obj2, SyncThread *obj3)
		{
			_objects.push_back(obj1);
			_objects.push_back(obj2);
			_objects.push_back(obj3);
		}

		/**
		 * \brief Constructor that accepts four thread objects and adds them to the inner BulkSyncThread list.
		 *
		 */
		BulkSyncThread(SyncThread *obj1, SyncThread *obj2, SyncThread *obj3, SyncThread *obj4)
		{
			_objects.push_back(obj1);
			_objects.push_back(obj2);
			_objects.push_back(obj3);
			_objects.push_back(obj4);
		}

		/**
		 * \brief Adds the thread object to the inner BulkSyncThread list.
		 *
		 */
		void operator+=(SyncThread *obj)
		{
			_objects.push_back(obj);
		}

		/**
		 * \brief Counts amount of thread objects in an external containter that are in the given state.
		 *
		 * \param begin the first iterator of the input container.
		 * \param end the last iterator of the input container.
		 * \param state state to count
		 * \return amount of thread objects in the external containter that are in the given state
		 */
		template<typename It> size_t Count(It begin, It end, jsyncthread_state_t state)
		{
			size_t rc = 0;

			for (; begin != end; ++begin) {
				if (get_ptr(*begin)->State() == state) {
					++rc;
				}
			}

			return rc;
		}

		/**
		 * \brief Perform launch operation on an external container of the thread objects. The method is
		 * blocking. The container can be a container of objects, pointers or boost's shared pointers.
		 *
		 * \param begin the first iterator of the input container.
		 * \param end the last iterator of the input container.
		 */
		template<typename It> static void Launch(It begin, It end)
		{
			std::list<SyncThread*> waiting_list;

			for (; begin != end; ++begin) {
				LaunchItem(get_ptr(*begin), waiting_list);
			}

			for (std::list<SyncThread*>::iterator i=waiting_list.begin(); i!=waiting_list.end(); i++) {
				(*i)->WaitTillLaunched();
			}
		}

		/**
		 * \brief Perform stop operation on an external container of the thread objects. The method will 
		 * block till all objects in the container are in the <code>completed</code> state. The container 
		 * can be a container objects, pointers or boost's shared pointers.
		 *
		 * \param begin the first iterator of the input container.
		 * \param end the last iterator of the input container.
		 * \param force_interrupt forces using the boost's inerruption points:
		 * 		if the thread in waitng at any of the <i>boost's interruption points</i>, it will be 
		 * 		interrupted as well.
		 */
		template<typename It> static void Stop(It begin, It end, bool force_interrupt=false)
		{
			for (It item=begin; item != end; ++item) {
				get_ptr(*item)->StopRequest(force_interrupt);
			}

			for (It item=begin; item != end; ++item) {
				get_ptr(*item)->WaitThread();
			}
		}

		/**
		 * \brief Perform pause operation on an external container of the thread objects. The method 
		 * is blocking. The container can be a container of objects, pointers or boost's shared pointers.
		 *
		 * \param begin the first iterator of the input container.
		 * \param end the last iterator of the input container.
		 */
		template<typename It> static void Pause(It begin, It end)
		{
			std::list<SyncThread*> waiting_list;

			for (; begin != end; ++begin) {
				PauseItem(get_ptr(*begin), waiting_list);
			}

			for (std::list<SyncThread*>::iterator i=waiting_list.begin(); i!=waiting_list.end(); i++) {
				(*i)->WaitTillPaused();
			}
		}

		/**
		 * \brief Perform resume operation on an external container of the thread objects. The method is 
		 * blocking. The container can be a container of objects, pointers or boost's shared pointers.
		 *
		 * \param begin the first iterator of the input container.
		 * \param end the last iterator of the input container.
		 */
		template<typename It> static void Resume(It begin, It end) 
		{
				std::list<SyncThread*> waiting_list;

				for (; begin != end; ++begin) {
					ResumeItem(get_ptr(*begin), waiting_list);
				}

				for (std::list<SyncThread*>::iterator i=waiting_list.begin(); i!=waiting_list.end(); i++) {
					(*i)->WaitTillResumed();
				}
		}

		/**
		 * \brief Counts amount of thread objects in the inner containter that are in the given state.
		 *
		 * \param state state to count
		 * \return amount of thread objects in the inner containter that are in the given state
		 */
		size_t Count(jsyncthread_state_t state)
		{
			return Count(_objects.begin(), _objects.end(), state);
		}

		/**
		 * \brief Perform launch operation on the inner thread object container. The method is blocking.
		 *
		 */
		void Launch()
		{
			return Launch(_objects.begin(), _objects.end());
		}

		/**
		 * \brief Perform stop operation on the inner thread object container. The method will block till 
		 * all objects in the container are in the <code>completed</code> state.
		 *
		 * \param force_interrupt forces using the boost's inerruption points:
		 * 		if the thread in waitng at any of the boost's inerruption points, it will be interrupted asa
		 * 		well.
		 */
		void Stop(bool force = false)
		{
			Stop(_objects.begin(), _objects.end(), force);
		}

		/**
		 * \brief Perform pause operation on the inner thread object container. The method is blocking.
		 *
		 *
		 */
		void Pause()
		{
			return Pause(_objects.begin(), _objects.end());
		}

		/**
		 * \brief Perform resume operation on the inner thread object container. The method is blocking.
		 *
		 *
		 */
		void Resume()
		{
			return Resume(_objects.begin(), _objects.end());
		}

};

}

#endif
