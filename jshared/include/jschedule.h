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
#ifndef J_PRIORITYSCHEDULE_H
#define J_PRIORITYSCHEDULE_H

#include "jobject.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#endif

#include <stdint.h>

namespace jshared {

/**
 * \brief Prioridade do escalonamento
 *
 */
enum jschedule_policy_t {
	SCHEDULE_FIFO,
	SCHEDULE_ROUND_ROBIN,
	SCHEDULE_OTHER
};

/**
 * \brief Prioridade do escalonamento
 *
 */
enum jschedule_type_t {
#ifdef _WIN32
	PRIORITY_PROCESS	= 0,
	PRIORITY_GROUP		= 0,
	PRIORITY_USER		= 0
#else
	PRIORITY_PROCESS	= PRIO_PROCESS,
	PRIORITY_GROUP		= PRIO_PGRP,
	PRIORITY_USER		= PRIO_USER
#endif
};

/**
 * \brief Parametros de escalonamento
 *
 */
struct jschedule_param_t {
	int priority;
};

/**
 * \brief Priority.
 *
 * \author Jeff Ferr
 */
class Schedule : public virtual jcommon::Object{

    private:
#ifdef _WIN32
        /** \brief */
		HANDLE _pid;
#else
        /** \brief */
		pid_t _pid;
#endif

	public:
		/**
		 * \brief Constructor.
		 *
		 */
#ifdef _WIN32
		Schedule(HANDLE pid_ = 0);
#else
		Schedule(pid_t pid_ = 0);
#endif
	
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Schedule();
		
		/**
		 * \brief
		 *
		 */
#ifdef _WIN32
		HANDLE GetPID();
#else
		pid_t GetPID();
#endif
		
		/**
		 * \brief
		 *
		 */
		void SetPriority(int n);

		/**
		 * \brief
		 *
		 */
		int GetCurrentPriority();
		
		/**
		 * \brief
		 *
		 */
		void SetSchedulerParameter(const jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		void GetSchedulerParameter(jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		void SetScheduler(jschedule_type_t policy_, const jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		jschedule_policy_t GetScheduler();
		
		/**
		 * \brief
		 *
		 */
		int GetMaximumPriority();
		
		/**
		 * \brief
		 *
		 */
		int GetMinimumPriority();
		
		/**
		 * \brief
		 *
		 */
		void SetScheduleAffinity(unsigned long mask_);
		
		/**
		 * \brief
		 *
		 */
		unsigned long GetScheduleAffinity();
		
		/**
		 * \brief
		 *
		 */
		void YieldProcess();
		
		/**
		 * \brief
		 *
		 */
		void IncreaseNice();
		
		/**
		 * \brief
		 *
		 */
		void DecreaseNice();
		
		/**
		 * \brief
		 *
		 */
		void SetPriority(int n, jschedule_type_t type_);

		/**
		 * \brief
		 *
		 */
		int GetPriority(jschedule_type_t type_);
		
};

}

#endif
