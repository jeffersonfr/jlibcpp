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
#include "jcommon/jtimer.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalstateexception.h"
#include "jexception/jillegalargumentexception.h"

#include <algorithm>

namespace jcommon {

/*
#include <iostream>
#include <chrono>
#include <thread>

template <typename Duration, typename Function> void timer(Duration const & d, Function const & f)
{
  std::thread([d,f](){
      std::this_thread::sleep_for(d);
      f();
      }).detach();
}

void hello() {std::cout << "Hello!\n";}

int main()
{
  timer(std::chrono::seconds(5), &hello);
  std::cout << "Launched\n";
  std::this_thread::sleep_for(std::chrono::seconds(10));
}
*/

static bool tasks_compare(TimerTask *a, TimerTask *b) 
{
	if (a->_next_execution_time < b->_next_execution_time) {
		return true;
	}

	return false;
}

TimerTask::TimerTask():
  jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::TimerTask");

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
  std::lock_guard<std::mutex> guard(_timertask_mutex);

	bool result = (_timertask_state == JTS_SCHEDULED);

	_timertask_state = JTS_CANCELLED;

	return result;
}

void TimerTask::Run()
{
}

TaskQueue::TaskQueue():
  jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::TaskQueue");
}

TaskQueue::~TaskQueue()
{
	Clear();
}

TimerTask * TaskQueue::GetMin() 
{
  std::lock_guard<std::mutex> guard(_mutex);

	if (_queue.size() == 0) {
		return NULL;
	}

	return _queue[0];
}

void TaskQueue::RemoveMin() 
{
  std::lock_guard<std::mutex> guard(_mutex);

	if (_queue.size() == 0) {
		return;
	}

	_queue.erase(_queue.begin());

	std::sort(_queue.begin(), _queue.end(), tasks_compare);
}

void TaskQueue::Add(TimerTask *task) 
{
  std::lock_guard<std::mutex> guard(_mutex);

	_queue.push_back(task);

	std::sort(_queue.begin(), _queue.end(), tasks_compare);

	_condition.notify_all();
}

void TaskQueue::Remove(TimerTask *task) 
{
  std::lock_guard<std::mutex> guard(_mutex);

	std::vector<TimerTask *>::iterator i = std::find(_queue.begin(), _queue.end(), task);

	if (i != _queue.end()) {
		_queue.erase(i);
	}

	std::sort(_queue.begin(), _queue.end(), tasks_compare);

	_condition.notify_all();
}

bool TaskQueue::IsEmpty() 
{
  std::lock_guard<std::mutex> guard(_mutex);
	
	return (_queue.size() == 0);
}

void TaskQueue::Clear() 
{
  std::lock_guard<std::mutex> guard(_mutex);

	_queue.clear();
}

int TaskQueue::GetSize() 
{
  std::lock_guard<std::mutex> guard(_mutex);

	return _queue.size();
}

void TaskQueue::RescheduleMin(uint64_t newTime) 
{
  std::lock_guard<std::mutex> guard(_mutex);

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

void TimerThread::Start()
{
  if (_is_running == true) {
    return;
  }

	_thread = std::thread(&TimerThread::Run, this);
}

void TimerThread::Stop()
{
  if (_is_running == false) {
    return;
  }

	_is_running = false;
	_new_tasks_may_be_scheduled = false;
	
	_queue->_condition.notify_all();

	_thread.join();
}

void TimerThread::MainLoop() 
{
	while (_is_running) {
		TimerTask *task = NULL;
		bool taskFired = false;

  	// std::lock_guard<std::mutex> guard(_mutex);
    std::unique_lock<std::mutex> guard(_mutex);

		// Wait for queue to become non-empty
		while (_queue->IsEmpty() && _new_tasks_may_be_scheduled) {
			_queue->_condition.wait(guard);
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

		uint64_t 
      currentTime = (uint64_t)jcommon::Date::CurrentTimeMicros(),
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
        std::chrono::microseconds duration(executionTime - currentTime);

				_queue->_condition.wait_for(guard, duration);
			} catch (jexception::Exception &) {
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
	} catch(jexception::Exception &) {
	}

	// Somone killed this Thread, behave as if Timer cancelled
 	std::lock_guard<std::mutex> guard(_mutex);

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
 	std::lock_guard<std::mutex> guard(_mutex);

	delete _thread;
	_thread = NULL;

	delete _queue;
	_queue = NULL;
}

void Timer::InternalSchedule(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	if (!_thread->_new_tasks_may_be_scheduled) {
		throw jexception::IllegalStateException("Timer already cancelled.");
	}

	if (task->_timertask_state != JTS_VIRGIN) {
		throw jexception::IllegalStateException("Task already scheduled or cancelled");
	}

	task->_next_execution_time = next_execution_time;
	task->_timertask_delay = delay;
	task->_timertask_state = JTS_SCHEDULED;
	task->_timertask_push_time = push_time;

	_queue->Add(task);

	if (_queue->GetMin() == task) {
		_queue->_condition.notify_all();
	}
}

void Timer::Start()
{
 	std::lock_guard<std::mutex> guard(_mutex);

	_thread->Start();
}

void Timer::Stop()
{
 	std::lock_guard<std::mutex> guard(_mutex);

	_thread->Stop();
}

void Timer::Schedule(TimerTask *task, uint64_t next_execution_time, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	InternalSchedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, 0LL, push_time);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *next_execution_time, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	InternalSchedule(task, next_execution_time->GetTime(), 0LL, push_time);
}

void Timer::Schedule(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	InternalSchedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, delay, push_time);
}

void Timer::Schedule(TimerTask *task, jcommon::Date *next_execution_time, uint64_t delay, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	if (delay <= 0LL) {
		throw jexception::IllegalArgumentException("Non-positive period.");
	}

	InternalSchedule(task, next_execution_time->GetTime(), delay, push_time);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, uint64_t next_execution_time, uint64_t delay, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	if (delay <= 0LL) {
		throw jexception::IllegalArgumentException("Non-positive period.");
	}

	InternalSchedule(task, jcommon::Date::CurrentTimeMicros()+next_execution_time, delay, push_time);
}

void Timer::ScheduleAtFixedRate(TimerTask *task, jcommon::Date *next_execution_time, uint64_t delay, bool push_time) 
{
 	std::lock_guard<std::mutex> guard(_mutex);

	InternalSchedule(task, next_execution_time->GetTime(), delay, push_time);
}

void Timer::RemoveSchedule(TimerTask *task)
{
 	std::lock_guard<std::mutex> guard(_mutex);

	_queue->Remove(task);
}

}

