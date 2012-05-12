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
#include "jtimer.h"
#include "jruntimeexception.h"

static bool tasks_compare(jthread::TimerTask *a, jthread::TimerTask *b) 
{
	if (a->_next_execution_time < b->_next_execution_time) {
		return true;
	}

	return false;
}

namespace jthread {

TimerTask::TimerTask() 
{
	_timertask_state = JTS_VIRGIN;
	_next_execution_time = 0LL;
	_timertask_delay = 0LL;
	_timertask_push_time = false;
}

TimerTask::~TimerTask() 
{
}

uint64_t TimerTask::GetDelay()
{
	return _timertask_delay;
}

void TimerTask::SetDelay(uint64_t delay)
{
	_timertask_delay = delay;
}

bool TimerTask::Cancel() 
{
	jthread::AutoLock lock(&_timertask_mutex);

	bool result = (_timertask_state == JTS_SCHEDULED);

	_timertask_state = JTS_CANCELLED;

	return result;
}

void TimerTask::Run()
{
}

TaskQueue::TaskQueue()
{
}

TaskQueue::~TaskQueue()
{
	Clear();
}

TimerTask * TaskQueue::GetMin() 
{
	jthread::AutoLock lock(&_mutex);

	if (_queue.size() == 0) {
		return NULL;
	}

	return _queue[0];
}

void TaskQueue::RemoveMin() 
{
	jthread::AutoLock lock(&_mutex);

	if (_queue.size() == 0) {
		return;
	}

	_queue.erase(_queue.begin());

	std::sort(_queue.begin(), _queue.end(), tasks_compare);
}

void TaskQueue::Add(TimerTask *task) 
{
	jthread::AutoLock lock(&_mutex);

	_queue.push_back(task);

	std::sort(_queue.begin(), _queue.end(), tasks_compare);

	_sem.Notify();
}

void TaskQueue::Remove(TimerTask *task) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<TimerTask *>::iterator i = std::find(_queue.begin(), _queue.end(), task);

	if (i != _queue.end()) {
		_queue.erase(i);
	}

	std::sort(_queue.begin(), _queue.end(), tasks_compare);

	_sem.Notify();
}

bool TaskQueue::IsEmpty() 
{
	jthread::AutoLock lock(&_mutex);
	
	return (_queue.size() == 0);
}

void TaskQueue::Clear() 
{
	jthread::AutoLock lock(&_mutex);

	_queue.clear();
}

int TaskQueue::GetSize() 
{
	jthread::AutoLock lock(&_mutex);

	return _queue.size();
}

void TaskQueue::RescheduleMin(uint64_t newTime) 
{
	jthread::AutoLock lock(&_mutex);

	if (_queue.size() == 0) {
		return;
	}

	_queue[0]->_next_execution_time = newTime;

	std::sort(_queue.begin(), _queue.end(), tasks_compare);
}

TimerThread::TimerThread(TaskQueue *queue) 
{
	_queue = queue;
	_is_running = true;
	_new_tasks_may_be_scheduled = true;
}

TimerThread::~TimerThread()
{
}

void TimerThread::Release()
{
	_is_running = false;
	_new_tasks_may_be_scheduled = false;
	
	_queue->_sem.Notify();

	WaitThread();
}

void TimerThread::MainLoop() 
{
	while (_is_running) {
		TimerTask *task = NULL;
		bool taskFired = false;

		jthread::AutoLock lock(&_mutex);

		// Wait for queue to become non-empty
		while (_queue->IsEmpty() && _new_tasks_may_be_scheduled) {
			_queue->_sem.Wait();
		}

		// Queue is empty and will forever remain; die
		if (_is_running == false || _queue->IsEmpty()) {
			break; 
		}

		// Queue nonempty; look at first evt and do the right thing
		task = _queue->GetMin();

		// monitor enter
		if (task == NULL || task->_timertask_state == JTS_CANCELLED) {
			if (task != NULL) {
				_queue->RemoveMin();
			}

			continue;  // No action required, poll queue again
		}

		uint64_t currentTime = (uint64_t)jcommon::Date::CurrentTimeMicros(),
						 executionTime = (uint64_t)task->_next_execution_time;

		taskFired = (executionTime <= currentTime);

		if (taskFired == true) {
			if (task->_timertask_delay == 0LL) { 
				// Non-repeating, remove
				_queue->RemoveMin();
				task->_timertask_state = JTS_EXECUTED;
			} else {
				// Repeating task, reschedule
				_queue->RescheduleMin(task->_timertask_push_time == true ? currentTime + task->_timertask_delay : executionTime + task->_timertask_delay);
			}
		}

		// Task hasn't yet fired; wait
		if (!taskFired) {
			try {
				_queue->_sem.Wait(executionTime - currentTime);
			} catch (jcommon::RuntimeException &) {
			}
		}

		// Task fired; run it, holding no locks
		if (_is_running == true && taskFired) {
			task->Run();
		}
	}
}

void TimerThread::Run() 
{
	try {
		MainLoop();
	} catch(ThreadException e) {
	}

	// Somone killed this Thread, behave as if Timer cancelled
	jthread::AutoLock lock(&_mutex);

	_new_tasks_may_be_scheduled = false;
	
	_queue->Clear();  // Eliminate obsolete references
}

Timer::Timer() 
{
	_queue = new TaskQueue();
	_thread = new TimerThread(_queue);
}

Timer::~Timer() 
{
	jthread::AutoLock lock(&_mutex);

	delete _thread;
	_thread = NULL;

	delete _queue;
	_queue = NULL;
}

void Timer::schedule(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	if (!_thread->_new_tasks_may_be_scheduled) {
		throw jthread::IllegalStateException("Timer already cancelled.");
	}

	if (task->_timertask_state != JTS_VIRGIN) {
		throw jthread::IllegalStateException("Task already scheduled or cancelled");
	}

	task->_next_execution_time = next_execution_time;
	task->_timertask_delay = delay;
	task->_timertask_state = JTS_SCHEDULED;
	task->_timertask_push_time = push_time;

	_queue->Add(task);

	if (_queue->GetMin() == task) {
		_queue->_sem.Notify();
	}
}

void Timer::Start()
{
	jthread::AutoLock lock(&_mutex);

	if (_thread->IsRunning() == false) {
		_thread->Start();
	}
}

void Timer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	_thread->Release();
}

void Timer::Schedule(TimerTask *task, uint64_t next_execution_time, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	schedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, 0LL, push_time);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *next_execution_time, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	schedule(task, next_execution_time->GetTime(), 0LL, push_time);
}

void Timer::Schedule(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	schedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, delay, push_time);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *next_execution_time, uint64_t delay, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	if (delay <= 0LL) {
		throw jcommon::IllegalArgumentException("Non-positive period.");
	}

	schedule(task, next_execution_time->GetTime(), delay, push_time);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	if (delay <= 0LL) {
		throw jcommon::IllegalArgumentException("Non-positive period.");
	}

	schedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, delay, push_time);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, jcommon::Date *next_execution_time, uint64_t delay, bool push_time) 
{
	jthread::AutoLock lock(&_mutex);

	schedule(task, next_execution_time->GetTime(), delay, push_time);
}

void Timer::RemoveSchedule(TimerTask *task)
{
	jthread::AutoLock lock(&_mutex);

	_queue->Remove(task);
}

}

