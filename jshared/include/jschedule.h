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

#include "jprocess.h"

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
	JSP_FIFO,
	JSP_ROUND_ROBIN,
	JSP_OTHER
};

/**
 * \brief Prioridade do escalonamento
 *
 */
enum jschedule_type_t {
#ifdef _WIN32
	PRIORITY_PROCESS	= 0,
	PRIORITY_GROUP		= 0,
	PRIORITY_USER			= 0
#else
	PRIORITY_PROCESS	= PRIO_PROCESS,
	PRIORITY_GROUP		= PRIO_PGRP,
	PRIORITY_USER			= PRIO_USER
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
       /** \brief */
			jpid_t _pid;

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		Schedule(jpid_t pid_ = 0);
	
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Schedule();
		
		/**
		 * \brief
		 *
		 */
		virtual jpid_t GetPID();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPriority(int n);

		/**
		 * \brief
		 *
		 */
		virtual int GetCurrentPriority();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSchedulerParameter(const jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetSchedulerParameter(jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScheduler(jschedule_type_t policy_, const jschedule_param_t *param_);
		
		/**
		 * \brief
		 *
		 */
		virtual jschedule_policy_t GetScheduler();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMaximumPriority();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMinimumPriority();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScheduleAffinity(unsigned long mask_);
		
		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetScheduleAffinity();
		
		/**
		 * \brief
		 *
		 */
		virtual void YieldProcess();
		
		/**
		 * \brief
		 *
		 */
		virtual void IncreaseNice();
		
		/**
		 * \brief
		 *
		 */
		virtual void DecreaseNice();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPriority(int n, jschedule_type_t type_);

		/**
		 * \brief
		 *
		 */
		virtual int GetPriority(jschedule_type_t type_);
		
};

}

#endif
