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
#ifndef J_PRIVATEPROCESS_H
#define J_PRIVATEPROCESS_H

#include "jobject.h"

#include <iostream>

#include <stdint.h>

namespace jshared {

enum jprocess_flag_t {
	PROCESS_BLOCKING,	/** \brief Create a new process */
	PROCESS_NONBLOCK	/** \brief Create a new process and block parent */
};

enum jprocess_type_t {
	PROCESS_CHILD,
	PROCESS_PARENT
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class PrivateProcess : public virtual jcommon::Object{

    private:
#ifdef _WIN32
        /** \brief Socket handler. */
		HANDLE _pid;
#else
        /** \brief Socket handler. */
        pid_t _pid;
#endif
        /** \brief */
		bool _is_running;
        /** \brief */
		int _timeout;
        /** \brief */
		jprocess_flag_t _flag;

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		PrivateProcess(jprocess_flag_t flag_);
	
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~PrivateProcess();
		
		/**
		 * \brief
		 *
		 */
		jprocess_type_t CreateProcess();
		
		/**
		 * \brief
		 *
		 */
		void WaitProcess();
		
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
#ifdef _WIN32
		HANDLE GetParentPID();
#else
		pid_t GetParentPID();
#endif
		
		/**
		 * \brief
		 *
		 */
		bool IsRunning();
		
		/**
		 * \brief 
		 *
		 */
		void Interrupt();
		
};

}

#endif
