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
 *a**************************************************************************/
#include "Stdafx.h"
#include "jschedule.h"
#include "jprocessexception.h"

namespace jshared {

#ifdef _WIN32
Schedule::Schedule(HANDLE pid_):
#else	
Schedule::Schedule(pid_t pid_):
#endif
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::Schedule");
	
#ifdef _WIN32
#else	
	if (pid_ < 1) {
		_pid = pid_;
	} else {
		_pid = getpid();
	}
#endif
}

Schedule::~Schedule()
{
}

#ifdef _WIN32
HANDLE Schedule::GetPID()
#else
pid_t Schedule::GetPID()
#endif
{
	return _pid;
}

void Schedule::SetSchedulerParameter(const jschedule_param_t *param_)
{
#ifdef _WIN32
#else	
	struct sched_param param;

	param.sched_priority = param_->priority;
	
	int r = sched_setparam(_pid, &param);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else {
			throw ProcessException("Schedule parameter exception");
		}
	}
#endif
}

void Schedule::GetSchedulerParameter(jschedule_param_t *param_)
{
#ifdef _WIN32
#else	
	struct sched_param param;

	param.sched_priority = param_->priority;
	
	int r = sched_getparam(_pid, &param);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else {
			throw ProcessException("Schedule parameter exception");
		}
	}

	param_->priority = param.sched_priority;
#endif
}

void Schedule::SetScheduler(jschedule_type_t type_, const jschedule_param_t *param_)
{
#ifdef _WIN32
#else	
	struct sched_param param;

	param.sched_priority = param_->priority;

	int type = 0;

	if (type_ == PRIORITY_PROCESS) {
		type = PRIO_PROCESS;
	} else if (type_ == PRIORITY_GROUP) {
		type = PRIO_PGRP;
	} else if (type_ == PRIORITY_USER) {
		type = PRIO_USER;
	}

	int r = sched_setscheduler(_pid, type, &param);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else {
			throw ProcessException("Schedule parameter exception");
		}
	}
#endif
}

jschedule_policy_t Schedule::GetScheduler()
{
#ifdef _WIN32
	return JSP_FIFO;
#else	
	int r = sched_getscheduler(_pid);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else {
			throw ProcessException("Schedule parameter exception");
		}
	}

	if (r == SCHED_FIFO) {
		return JSP_FIFO;
	} else if (r == SCHED_RR) {
		return JSP_ROUND_ROBIN;
	} else {
		return JSP_OTHER;
	}
#endif
}

int Schedule::GetMaximumPriority()
{
#ifdef _WIN32
	return 0;
#else	
	int r = sched_get_priority_max(GetScheduler());

	if (r < 0) {
		throw ProcessException("Getting maximum priority failed");
	}

	return r;
#endif
}

int Schedule::GetMinimumPriority()
{
#ifdef _WIN32
	return 0;
#else	
	int r = sched_get_priority_min(GetScheduler());

	if (r < 0) {
		throw ProcessException("Getting minimum priority failed");
	}

	return r;
#endif
}

void Schedule::SetScheduleAffinity(unsigned long mask)
{
#ifdef _WIN32
#else
	int r = sched_setaffinity(_pid, 1, (cpu_set_t *)&mask);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else if (errno == EINVAL) {
			throw ProcessException("Mask contains no processors that are physically on the system");
		} else {
			throw ProcessException("Set schedule affinity failed");
		}
	}
#endif
}

unsigned long Schedule::GetScheduleAffinity()
{
#ifdef _WIN32
	return 0;
#else	
	unsigned long mask;
	
	int r = sched_setaffinity(_pid, 1, (cpu_set_t *)&mask);

	if (r < 0) {
		if (errno == EPERM) {
			throw ProcessException("Process does not have appropriate privileges");
		} else if (errno == EINVAL) {
			throw ProcessException("Mask contains no processors that are physically on the system");
		} else {
			throw ProcessException("Set schedule affinity failed");
		}
	}

	return mask;
#endif
}

void Schedule::YieldProcess()
{
#ifdef _WIN32
#else	
	int r = sched_yield();

	if (r < 0) {
		throw ProcessException("Yield failed");
	}
#endif
}

void Schedule::IncreaseNice()
{
#ifdef _WIN32
#else	
	int r = nice(-1);

	if (r < 0) {
		throw ProcessException("Only root may specify the priority");
	}
#endif
}

void Schedule::DecreaseNice()
{
#ifdef _WIN32
#else	
	int r = nice(1);

	if (r < 0) {
		throw ProcessException("Only root may specify the priority");
	}
#endif
}

void Schedule::SetPriority(int n, jschedule_type_t type_)
{
#ifdef _WIN32
#else	
	if (n < GetMinimumPriority() || n > GetMaximumPriority()) {
		throw ProcessException("Range of priority error");
	}

	int type = 0;

	if (type_ == PRIORITY_PROCESS) {
		type = PRIO_PROCESS;
	} else if (type_ == PRIORITY_GROUP) {
		type = PRIO_PGRP;
	} else if (type_ == PRIORITY_USER) {
		type = PRIO_USER;
	}

	// valores de prioridade baixos aumentam a preferencia do escalonador
	setpriority(type, 0, -n);
#endif
}

int Schedule::GetPriority(jschedule_type_t type_)
{
#ifdef _WIN32
	return 0;
#else	
	int type = 0;

	if (type_ == PRIORITY_PROCESS) {
		type = PRIO_PROCESS;
	} else if (type_ == PRIORITY_GROUP) {
		type = PRIO_PGRP;
	} else if (type_ == PRIORITY_USER) {
		type = PRIO_USER;
	}

	// valores de prioridade altos diminuem a preferencia do escalonador
	int r = getpriority(type, 0);

	if (r < 0) {
		if (errno == EACCES) {
			throw ProcessException("A non super-user attempted to lower a process priority");
		} else {
			throw ProcessException("Get current priority error");
		}
	}

	return r;
#endif
}

}
