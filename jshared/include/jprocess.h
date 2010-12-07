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
#ifndef J_PROCESS_H
#define J_PROCESS_H

#include "jprivateprocess.h"
#include "jinputstream.h"
#include "joutputstream.h"

#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#endif

#include <stdint.h>

namespace jshared {

enum jwprocess_mode_t {
	FLUSH_WRITE_BUFFER, 
	WAIT_ANY_INPUT, 
	WAIT_ALL_INPUT, 
	TRY_ONCE
};
  
enum jwprocess_flag_t {
	PROCESS_CREAT,	// \brief Create a new process
	PROCESS_LOCK		// \brief Create a new process and block parent
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class Process : public virtual jcommon::Object{

    private:
			/** \brief */
			jio::InputStream *_input;
			/** \brief */
			jio::OutputStream *_output;
			/** \brief */
			jio::InputStream *_error;
#ifdef _WIN32
			/** \brief */
			HANDLE _pid;
#else
			/** \brief */
			pid_t _pid;
#endif
			/** \brief */
			int _pinput[2],
					_poutput[2],
					_perror[2];
			/** \brief */
			bool _is_running;

		private:
			/**
			 * \brief 
			 *
			 */
			int MakeHandleGreaterThan2(int fd);

			/**
			 * \brief 
			 *
			 */
			void ForkChild(const char *prog, char **args);

		public:
			/**
			 * \brief Constructor.
			 *
			 */
			Process(std::string process);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~Process();

			/**
			 * \brief 
			 *
			 */
			jio::InputStream * GetInputStream();
			
			/**
			 * \brief 
			 *
			 */
			jio::OutputStream * GetOutputStream();
			
			/**
			 * \brief 
			 *
			 */
			jio::InputStream * GetErrorStream();
			
			/**
			 * \brief
			 *
			 */
			void Interrupt();

			/**
			 * \brief
			 *
			 */
			void WaitProcess();

};

}

#endif
