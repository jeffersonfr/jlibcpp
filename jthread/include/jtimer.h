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
#ifndef J_TIMER_H
#define J_TIMER_H

#include "jthread.h"
#include "jsemaphore.h"
#include "jautolock.h"
#include "jdate.h"
#include "jillegalargumentexception.h"
#include "jillegalstateexception.h"

#include <map>

namespace jthread{

/**
 * \brief Thread.
 *
 * @author Jeff Ferr
 */
enum jtimertask_state_t {
	JTS_VIRGIN,			// The state of this task, chosen from the constants below. This task has not yet been scheduled.
	JTS_SCHEDULED,	// This task is scheduled for execution.  If it is a non-repeating task, it has not yet been executed.
	JTS_EXECUTED,		// This non-repeating task has already executed (or is currently executing) and has not been cancelled.
	JTS_CANCELLED		// This task has been cancelled (with a call to TimerTask.cancel).
};

class Timer;
class TimerThread;
class TaskQueue;

class TimerTask : public jthread::Runnable {

	friend class Timer;
	friend class TimerThread;
	friend class TaskQueue;

	private:
		jthread::Mutex _mutex;
    jtimertask_state_t _state;
    
		uint64_t _delay;
		bool _push_time;

	protected:
    /**
     * \brief Creates a new timer task.
		 *
     */
    TimerTask();

	public:
		uint64_t _next_execution_time;

	public:
    /**
     * \brief 
		 *
     */
    virtual ~TimerTask();

		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetDelay();

		/**
		 * \brief
		 *
		 */
		virtual void SetDelay(uint64_t delay);

    /**
     * Cancels this timer task.  If the task has been scheduled for one-time execution and has not yet run, or 
		 * has not yet been scheduled, it will never run.  If the task has been scheduled for repeated execution, 
		 * it will never run again.  (If the task is running when this call occurs, the task will run to completion, 
		 * but will never run again.)
     *
     * <p>Note that calling this method from within the <tt>run</tt> method of a repeating timer task absolutely 
		 * guarantees that the timer task will not run again.
     *
     * <p>This method may be called repeatedly; the second and subsequent calls have no effect.
     *
     * @return true if this task is scheduled for one-time execution and has not yet run, or this task is scheduled 
		 * for repeated execution. Returns false if the task was scheduled for one-time execution and has already run, 
		 * or if the task was never scheduled, or if the task was already cancelled.  (Loosely speaking, this method
		 * returns <tt>true</tt> if it prevents one or more scheduled executions from taking place.)
     */
    virtual bool Cancel();

		/**
     * \brief The action to be performed by this timer task.
		 *
     */
    virtual void Run();

};

/**
 * \brief This class represents a timer task queue: a priority queue of TimerTasks, ordered on nextExecutionTime.  
 * Each Timer object has one of these, which it shares with its TimerThread.  Internally this class uses a heap, 
 * which offers log(n) performance for the add, removeMin and rescheduleMin operations, and constant time performance 
 * for the the getMin operation.
 */
class TaskQueue {

	friend class Timer;
	friend class TimerThread;

	private:
		std::vector<TimerTask *> _queue;
		jthread::Mutex _mutex;
		jthread::Semaphore _sem;

	private:
		/**
		 * \brief Return the "head task" of the priority queue.  (The head task is an task with the lowest nextExecutionTime.)
		 *
		 */
		TimerTask * GetMin();

		/**
		 * \brief Remove the head task from the priority queue.
		 *
		 */
		void RemoveMin();

	protected:
		/**
		 * \brief
		 *
		 */
		TaskQueue();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~TaskQueue();

		/**
		 * \brief Adds a new task to the priority queue.
		 *
		 */
		void Add(TimerTask *task);

		/**
		 * \brief Adds a new task to the priority queue.
		 *
		 */
		void Remove(TimerTask *task);

		/**
		 * \brief Returns true if the priority queue contains no elements.
		 *
		 */
		bool IsEmpty();

		/**
		 * \brief Removes all elements from the priority queue.
		 *
		 */
		void Clear();

		/**
		 * \brief
		 *
		 */
		int GetSize();

		/**
		 * \brief Sets the nextExecutionTime associated with the head task to the specified value, and adjusts priority 
		 * queue accordingly.
		 *
		 */
		void RescheduleMin(uint64_t newTime);

};

/**
 * \brief This "helper class" implements the timer's task execution thread, which waits for tasks on the timer queue, 
 * executions them when they fire, reschedules repeating tasks, and removes cancelled tasks and spent non-repeating 
 * tasks from the queue.
 *
 */
class TimerThread : public jthread::Thread {

	friend class Timer;

	private:
		jthread::Mutex _mutex;
   	TaskQueue *_queue;
    bool _is_running,
				 _new_tasks_may_be_scheduled;

	private:
		/**
     * \brief The main timer loop.
		 *
     */
		void MainLoop();

	protected:
		/**
		 * \brief
		 *
		 */
    TimerThread(TaskQueue *queue);

	public:
		/**
		 * \brief
		 *
		 */
    virtual ~TimerThread();

		/**
		 * \brief
		 *
		 */
    virtual void Release();

		/**
		 * \brief
		 *
		 */
    virtual void Run();

};

class Timer : public virtual jcommon::Object{

	private:
		jthread::Mutex _mutex;
    TimerThread *_thread;
    TaskQueue *_queue;

	private:
    /**
     * \brief Schedule the specifed timer task for execution at the specified time with the specified period, in microseconds.  
		 * If period is positive, the task is scheduled for repeated execution; if period is zero, the task is scheduled for one-time 
		 * execution. Time is specified in Date.getTime() format.  This method checks timer state, task state, and initial execution 
		 * time, but not period.
     */
    void schedule(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time);

	public:
    /**
     * \brief Creates a new timer.  The associated thread does <i>not</i> run as a daemon.
     *
     */
    Timer();

    /**
     * \brief Terminates this timer, discarding any currently scheduled tasks. Does not interfere with a currently 
		 * executing task (if it exists).Once a timer has been terminated, its execution thread terminates gracefully, 
		 * and no more tasks may be scheduled on it.
     *
     * <p>Note that calling this method from within the run method of a timer task that was invoked by this timer 
		 * absolutely guarantees that the ongoing task execution is the last task execution that will ever be performed 
		 * by this timer.
     *
     * <p>This method may be called repeatedly; the second and subsequent calls have no effect.
     */
    virtual ~Timer();

    /**
     * \brief Schedules the specified task for execution after the specified delay.
     *
     * @param task  task to be scheduled.
     * @param delay delay in microseconds before task is to be executed.
     */
    void Schedule(TimerTask *task, uint64_t time, bool delay);

    /**
     * \brief Schedules the specified task for execution at the specified time.  If the time is in the past, 
		 * the task is scheduled for immediate execution.
     *
     * @param task task to be scheduled.
     * @param time time at which task is to be executed.
     */
    void Schedule(TimerTask *task, jcommon::Date *time, bool delay);

    /**
     * \brief Schedules the specified task for repeated <i>fixed-delay execution</i>, beginning after the 
		 * specified delay.  Subsequent executions take place at approximately regular intervals separated by the 
		 * specified period.
     *
     * <p>In fixed-delay execution, each execution is scheduled relative to the actual execution time of the 
		 * previous execution.  If an execution is delayed for any reason (such as garbage collection or other
		 * background activity), subsequent executions will be delayed as well. In the long run, the frequency of 
		 * execution will generally be slightly lower than the reciprocal of the specified period (assuming the system
     * clock underlying <tt>Object.wait(long)</tt> is accurate).
     *
     * <p>Fixed-delay execution is appropriate for recurring activities that require "smoothness."  In other 
		 * words, it is appropriate for activities where it is more important to keep the frequency accurate in the 
		 * short run than in the long run.  This includes most animation tasks, such as blinking a cursor at regular 
		 * intervals.  It also includes tasks wherein regular activity is performed in response to human input, such 
		 * as automatically repeating a character as long as a key is held down.
     *
     * @param task   task to be scheduled.
     * @param delay  delay in microseconds before task is to be executed.
     * @param period time in microseconds between successive task executions.
     */
    void Schedule(TimerTask *task, uint64_t time, uint64_t period, bool delay);

    /**
     * \brief Schedules the specified task for repeated <i>fixed-delay execution</i>, beginning at the specified 
		 * time. Subsequent executions take place at approximately regular intervals, separated by the specified period.
     *
     * <p>In fixed-delay execution, each execution is scheduled relative to the actual execution time of the previous 
		 * execution.  If an execution is delayed for any reason (such as garbage collection or other background activity), 
		 * subsequent executions will be delayed as well. In the long run, the frequency of execution will generally be 
		 * slightly lower than the reciprocal of the specified period (assuming the system clock underlying
		 * <tt>Object.wait(long)</tt> is accurate).
     *
     * <p>Fixed-delay execution is appropriate for recurring activities that require "smoothness."  In other words, 
		 * it is appropriate for activities where it is more important to keep the frequency accurate in the short run 
		 * than in the long run.  This includes most animation tasks, such as blinking a cursor at regular intervals. It 
		 * also includes tasks wherein regular activity is performed in response to human input, such as automatically 
		 * repeating a character as long as a key is held down.
     *
     * @param task   task to be scheduled.
     * @param firstTime First time at which task is to be executed.
     * @param period time in microseconds between successive task executions.
     */
    void Schedule(TimerTask *task, jcommon::Date *time, uint64_t period, bool delay);

    /**
     * \brief Schedules the specified task for repeated <i>fixed-rate execution</i>, beginning after the specified 
		 * delay.  Subsequent executions take place at approximately regular intervals, separated by the specified period.
     *
     * <p>In fixed-rate execution, each execution is scheduled relative to the scheduled execution time of the initial
		 * execution.  If an execution is delayed for any reason (such as garbage collection or other background activity), 
		 * two or more executions will occur in rapid succession to "catch up."  In the long run, the frequency of execution 
		 * will be exactly the reciprocal of the specified period (assuming the system clock underlying 
		 * <tt>Object.wait(long)</tt> is accurate).
     *
     * <p>Fixed-rate execution is appropriate for recurring activities that are sensitive to <i>absolute</i> time, 
		 * such as ringing a chime every hour on the hour, or running scheduled maintenance every day at a particular time.  
		 * It is also appropriate for for recurring activities where the total time to perform a fixed number of executions 
		 * is important, such as a countdown timer that ticks once every second for ten seconds.  Finally, fixed-rate 
		 * execution is appropriate for scheduling multiple repeating timer tasks that must remain synchronized with respect 
		 * to one another.
     *
     * @param task   task to be scheduled.
     * @param delay  delay in microseconds before task is to be executed.
     * @param period time in microseconds between successive task executions.
     */
    void ScheduleAtFixedRate(TimerTask *task, uint64_t time, uint64_t period, bool delay);

    /**
     * Schedules the specified task for repeated <i>fixed-rate execution</i>, beginning at the specified time. Subsequent 
		 * executions take place at approximately regular intervals, separated by the specified period.
     *
     * <p>In fixed-rate execution, each execution is scheduled relative to the scheduled execution time of the initial 
		 * execution.  If an execution is delayed for any reason (such as garbage collection or other background activity), 
		 * two or more executions will occur in rapid succession to "catch up."  In the long run, the frequency of execution 
		 * will be exactly the reciprocal of the specified period (assuming the system clock underlying 
		 * <tt>Object.wait(long)</tt> is accurate).
     *
     * <p>Fixed-rate execution is appropriate for recurring activities that are sensitive to <i>absolute</i> time, such 
		 * as ringing a chime every hour on the hour, or running scheduled maintenance every day at a particular time.  It is 
		 * also appropriate for for recurring activities where the total time to perform a fixed number of executions is important, 
		 * such as a countdown timer that ticks once every second for ten seconds.  Finally, fixed-rate execution is appropriate 
		 * for scheduling multiple repeating timer tasks that must remain synchronized with respect to one another.
     *
     * @param task task to be scheduled.
     * @param firstTime First time at which task is to be executed.
     * @param period time in microseconds between successive task executions.
     */
    void ScheduleAtFixedRate(TimerTask *task, jcommon::Date *time, uint64_t period, bool delay);

		/**
		 * \brief Remove a schedule reference from timer.
		 *
		 */
		void RemoveSchedule(TimerTask *task);

};

}

#endif
