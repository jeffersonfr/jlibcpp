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
#include "jshared/jschedule.h"
#include "jexception/jprocessexception.h"

#include <sys/types.h>

#include <unistd.h>

namespace jshared {

Schedule::Schedule(pid_t pid_):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jshared::Schedule");
  
  _pid = pid_;

  if (pid_ < 1) {
    _pid = getpid();
  }
}

Schedule::~Schedule()
{
}

void Schedule::SetSchedulerParameter(const jschedule_param_t *param_)
{
  struct sched_param param;

  param.sched_priority = param_->priority;
  
  int r = sched_setparam(_pid, &param);

  if (r < 0) {
    if (errno == EPERM) {
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else {
      throw jexception::ProcessException("Schedule parameter exception");
    }
  }
}

void Schedule::GetSchedulerParameter(jschedule_param_t *param_)
{
  struct sched_param param;

  param.sched_priority = param_->priority;
  
  int r = sched_getparam(_pid, &param);

  if (r < 0) {
    if (errno == EPERM) {
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else {
      throw jexception::ProcessException("Schedule parameter exception");
    }
  }

  param_->priority = param.sched_priority;
}

void Schedule::SetScheduler(jschedule_type_t type_, const jschedule_param_t *param_)
{
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
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else {
      throw jexception::ProcessException("Schedule parameter exception");
    }
  }
}

jschedule_policy_t Schedule::GetScheduler()
{
  int r = sched_getscheduler(_pid);

  if (r < 0) {
    if (errno == EPERM) {
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else {
      throw jexception::ProcessException("Schedule parameter exception");
    }
  }

  if (r == SCHED_FIFO) {
    return JSP_FIFO;
  } else if (r == SCHED_RR) {
    return JSP_ROUND_ROBIN;
  } else {
    return JSP_OTHER;
  }
}

int Schedule::GetMaximumPriority()
{
  int r = sched_get_priority_max(GetScheduler());

  if (r < 0) {
    throw jexception::ProcessException("Getting maximum priority failed");
  }

  return r;
}

int Schedule::GetMinimumPriority()
{
  int r = sched_get_priority_min(GetScheduler());

  if (r < 0) {
    throw jexception::ProcessException("Getting minimum priority failed");
  }

  return r;
}

void Schedule::SetScheduleAffinity(uint64_t mask)
{
  int r = sched_setaffinity(_pid, 1, (cpu_set_t *)&mask);

  if (r < 0) {
    if (errno == EPERM) {
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else if (errno == EINVAL) {
      throw jexception::ProcessException("Mask contains no processors that are physically on the system");
    } else {
      throw jexception::ProcessException("Set schedule affinity failed");
    }
  }
}

uint64_t Schedule::GetScheduleAffinity()
{
  uint64_t mask;
  
  int r = sched_setaffinity(_pid, 1, (cpu_set_t *)&mask);

  if (r < 0) {
    if (errno == EPERM) {
      throw jexception::ProcessException("Process does not have appropriate privileges");
    } else if (errno == EINVAL) {
      throw jexception::ProcessException("Mask contains no processors that are physically on the system");
    } else {
      throw jexception::ProcessException("Set schedule affinity failed");
    }
  }

  return (uint64_t)mask;
}

void Schedule::YieldProcess()
{
  int r = sched_yield();

  if (r < 0) {
    throw jexception::ProcessException("Yield failed");
  }
}

void Schedule::IncreaseNice()
{
  int r = nice(-1);

  if (r < 0) {
    throw jexception::ProcessException("Only root may specify the priority");
  }
}

void Schedule::DecreaseNice()
{
  int r = nice(1);

  if (r < 0) {
    throw jexception::ProcessException("Only root may specify the priority");
  }
}

void Schedule::SetPriority(int n, jschedule_type_t type_)
{
  if (n < GetMinimumPriority() || n > GetMaximumPriority()) {
    throw jexception::ProcessException("Range of priority error");
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
}

int Schedule::GetPriority(jschedule_type_t type_)
{
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
      throw jexception::ProcessException("A non super-user attempted to lower a process priority");
    } else {
      throw jexception::ProcessException("Get current priority error");
    }
  }

  return r;
}

}
