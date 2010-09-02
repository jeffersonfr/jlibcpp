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
#include "jsemaphoretimeoutexception.h"

static bool tasks_compare(jthread::TimerTask *a, jthread::TimerTask *b) 
{
	if (a->nextExecutionTime < b->nextExecutionTime) {
		return true;
	}

	return false;
}

namespace jthread {

TimerTask::TimerTask() 
{
	state = VIRGIN;
	nextExecutionTime = 0LL;
	period = 0LL;
	delay = false;
}

TimerTask::~TimerTask() 
{
}

bool TimerTask::Cancel() 
{
	jthread::AutoLock lock(&_mutex);

	bool result = (state == SCHEDULED);

	state = CANCELLED;

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

	return _queue[0];
}

void TaskQueue::RemoveMin() 
{
	jthread::AutoLock lock(&_mutex);

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

	return _queue.empty();
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

	_queue[0]->nextExecutionTime = newTime;

	std::sort(_queue.begin(), _queue.end(), tasks_compare);
}

TimerThread::TimerThread(TaskQueue *queue) 
{
	_queue = queue;

	newTasksMayBeScheduled = true;
}

TimerThread::~TimerThread()
{
}

void TimerThread::MainLoop() 
{
	while (true) {
		TimerTask *task = NULL;
		bool taskFired = false;

		jthread::AutoLock lock(&_mutex);

		// Wait for queue to become non-empty
		while (_queue->IsEmpty() && newTasksMayBeScheduled) {
			_queue->_sem.Wait();
		}

		// Queue is empty and will forever remain; die
		if (_queue->IsEmpty()) {
			break; 
		}

		// Queue nonempty; look at first evt and do the right thing
		uint64_t currentTime, 
						 executionTime;

		task = _queue->GetMin();

		{
			// monitor enter
			if (task->state == CANCELLED) {
				_queue->RemoveMin();

				continue;  // No action required, poll queue again
			}

			currentTime = (uint64_t)jcommon::Date::CurrentTimeMicros();
			executionTime = (uint64_t)task->nextExecutionTime;

			taskFired = (executionTime <= currentTime);
			
			if (taskFired == true) {
				if (task->period == 0LL) { 
					// Non-repeating, remove
					_queue->RemoveMin();
					task->state = EXECUTED;
				} else {
					// Repeating task, reschedule
					_queue->RescheduleMin(task->delay == true ? currentTime + task->period : executionTime + task->period);
				}
			}
		}

		// Task hasn't yet fired; wait
		if (!taskFired) {
			try {
				_queue->_sem.Wait(executionTime - currentTime);
			} catch (SemaphoreTimeoutException &e) {
			}
		}

		// Task fired; run it, holding no locks
		if (taskFired) {
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

	newTasksMayBeScheduled = false;
	
	_queue->Clear();  // Eliminate obsolete references
}

void Timer::sched(TimerTask *task, uint64_t time, uint64_t period, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	if (!_thread->newTasksMayBeScheduled) {
		throw jthread::IllegalStateException("Timer already cancelled.");
	}

	if (task->state != VIRGIN) {
		throw jthread::IllegalStateException("Task already scheduled or cancelled");
	}

	task->nextExecutionTime = time;
	task->period = period;
	task->state = SCHEDULED;
	task->delay = false;

	_queue->Add(task);

	if (_queue->GetMin() == task) {
		_queue->_sem.Notify();
	}
}

Timer::Timer() 
{
	_queue = new TaskQueue();
	_thread = new TimerThread(_queue);

	_thread->Start();
}

Timer::~Timer() 
{
	jthread::AutoLock lock(&_mutex);

	_thread->newTasksMayBeScheduled = false;

	_queue->_sem.Notify(); // In case queue is empty.

	delete _thread;
	delete _queue;
}

void Timer::Schedule(TimerTask *task, uint64_t time, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	sched(task, jcommon::Date::CurrentTimeMicros()+time, 0LL, delay);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *time, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	sched(task, time->GetTime(), 0LL, delay);
}

void Timer::Schedule(TimerTask *task, uint64_t time, uint64_t period, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	sched(task, jcommon::Date::CurrentTimeMicros()+time, period, delay);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *time, uint64_t period, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	if (period <= 0LL) {
		throw jcommon::IllegalArgumentException("Non-positive period.");
	}

	sched(task, time->GetTime(), period, delay);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, uint64_t time, uint64_t period, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	if (period <= 0LL) {
		throw jcommon::IllegalArgumentException("Non-positive period.");
	}

	sched(task, jcommon::Date::CurrentTimeMicros()+time, period, delay);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, jcommon::Date *time, uint64_t period, bool delay) 
{
	jthread::AutoLock lock(&_mutex);

	sched(task, time->GetTime(), period, delay);
}

void Timer::RemoveSchedule(TimerTask *task)
{
	jthread::AutoLock lock(&_mutex);

	_queue->Remove(task);
}

void Timer::Cancel() 
{
	jthread::AutoLock lock(&_mutex);

	_thread->newTasksMayBeScheduled = false;
	
	_queue->Clear();
	
	_queue->_sem.Notify();  // In case queue was already empty.
}

}

