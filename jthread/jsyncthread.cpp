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
#include "jsyncthread.h"
#include "jthreadgroup.h"
#include "jthreadexception.h"
#include "jautolock.h"

namespace jthread {

SyncThread::SyncThread():
	jthread::Thread()
{
	jcommon::Object::SetClassName("jthread::SyncThread");

	m_state = JSS_INIT;
	m_request = JSR_NONE;
}

bool SyncThread::Launch()
{
	try {
		Thread::Start();
	} catch (ThreadException &e) {
		return false;
	}

	return true;
}

jsyncthread_state_t SyncThread::State() 
{
	return m_state;
}

bool SyncThread::Start()
{
	jsyncthread_status_t rc = StartEvent();

	if (rc.wait) {
		rc.success &= WaitTillLaunched();
	}

	return rc.success;
}

bool SyncThread::Pause()
{
	jsyncthread_status_t rc = PauseEvent();

	if (rc.wait) {
		rc.success &= WaitTillPaused();
	}

	return rc.success;
}

bool SyncThread::Resume()
{
	jsyncthread_status_t rc = ResumeEvent();

	if (rc.wait) {
		rc.success &= WaitTillResumed();
	}

	return rc.success;
}

void SyncThread::Stop(bool force)
{
	StopRequest(force);
}

jsyncthread_status_t SyncThread::StartEvent()
{
	// no re-run
	if (JSS_COMPLETED == m_state || JSS_PAUSED == m_state) {
		return jsyncthread_status_t(false);
	}

	// already runs
	if (JSS_RUNNING == m_state) {
		return jsyncthread_status_t(true, false);
	}

	// try to launch
	// the call may fail due to a system error
	return jsyncthread_status_t(Launch());
}

jsyncthread_status_t SyncThread::PauseEvent()
{
	// already JSS_PAUSED
	if (JSS_PAUSED == m_state) {
		return jsyncthread_status_t(true, false);
	}

	if (IsJoinable() == false) {
		return jsyncthread_status_t(false);
	}

	Request(JSR_PAUSE);

	// callback is called
	// after the flag is changed
	OnInterrupt();

	return jsyncthread_status_t(true);
}

jsyncthread_status_t SyncThread::ResumeEvent()
{
	// already resumed
	if (JSS_RUNNING == m_state) {
		return jsyncthread_status_t(true, false);
	}

	if (IsJoinable() == false) {
		return jsyncthread_status_t(false);
	}

	WakeUp(JSR_NONE);

	return jsyncthread_status_t(true);
}

void SyncThread::StopRequest(bool force)
{
	// do nothingfor JSS_COMPLETED state
	if (JSS_COMPLETED == m_state) {
		return;
	}

	// if the thread is not launched
	// just change the state
	if (JSS_INIT == m_state) {
		m_state = JSS_COMPLETED;

		return;
	}

	WakeUp(JSR_STOP);

	// callback is called after the flag is changed
	OnInterrupt();

	// interrupt waiting in any interruption point
	if (force == true) {
		Interrupt();

		m_interrupt.Wait();
	} else {
		WaitThread();
	}
	
	SignalState(JSS_COMPLETED);
}

void SyncThread::WakeUp(jsyncthread_request_t rq)
{
	// signal that the pause is over
	m_guard.Lock();

	m_request=rq;

	m_pause.Notify();
	m_guard.Unlock();
}

bool SyncThread::WaitTillLaunched()
{
	// wait till the thread is launched
	m_guard.Lock();

	while(JSS_RUNNING != m_state && JSS_COMPLETED != m_state) {
		m_signal.Wait(&m_guard);
	}

	m_guard.Unlock();

	if(JSS_RUNNING != m_state) {
		return false;
	}

	return true;
}

bool SyncThread::WaitTillResumed()
{
	// wait till the thread is resumed
	m_guard.Lock();

	while (JSS_RUNNING != m_state && JSS_COMPLETED != m_state) {
		m_signal.Wait(&m_guard);
	}

	m_guard.Unlock();

	return (JSS_RUNNING == m_state);
}

bool SyncThread::WaitTillPaused()
{
	// wait till the thread is JSS_PAUSED
	m_guard.Lock();

	while(JSS_PAUSED != m_state && JSS_COMPLETED != m_state) {
		m_signal.Wait(&m_guard);
	}

	m_guard.Unlock();

	return (JSS_PAUSED == m_state);
}

void SyncThread::Idle()
{
	// signal JSS_PAUSED state
	SignalState(JSS_PAUSED);

	// wait in the JSS_PAUSED state
	m_guard.Lock();

	while(JSR_PAUSE == m_request) {
		m_pause.Wait(&m_guard);
	}

	m_guard.Unlock();

	// signal JSS_RUNNING state
	SignalState(JSS_RUNNING);
}

void SyncThread::Run()
{
	try {
		SignalState(JSS_RUNNING);
		OnStart();

		while (JSR_STOP != m_request) {
			while (JSR_NONE == m_request) {
				if (Action() == false) {
					break;
				}
			}

			if (JSR_PAUSE != m_request) {
				break;
			}

			Idle();
		}

		SignalState(JSS_COMPLETED);
		OnExit();
	} catch (...) {
		m_interrupt.Notify();

		throw;
	}
}

void SyncThread::SignalState(jsyncthread_state_t state)
{
	m_guard.Lock();

	// update the state
	// and signal that the thread is 
	// in new state 
	m_state=state;

	m_signal.Notify();
	m_guard.Unlock();
}

void SyncThread::Request(jsyncthread_request_t rq)
{
	m_request = rq;
}

bool SyncThread::Action()
{
	return false;
}

void SyncThread::OnStart()
{
}

void SyncThread::OnExit()
{
}

void SyncThread::OnInterrupt()
{
}

bool SyncThread::IsInterrupted()
{
	return (JSR_NONE != m_request);
}

}
