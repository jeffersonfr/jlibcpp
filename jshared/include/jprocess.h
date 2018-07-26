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

#include "jio/jinputstream.h"
#include "jio/joutputstream.h"

#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>

namespace jshared {

enum jprocess_type_t {
	JPT_CHILD,
	JPT_PARENT
};

enum jprocess_mode_t {
	JPM_AUTO_FLUSH,
	JPM_WAIT_ANY_INPUT, 
	JPM_WAIT_ALL_INPUT, 
	JPM_TRY_ONCE
};
  
enum jprocess_flag_t {
	JPF_CREATE,	// \brief Create a new process
	JPF_LOCK		// \brief Create a new process and block parent
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
			/** \brief */
			std::string _process;
			/** \brief */
			pid_t _pid;
			/** \brief */
			int _pinput[2];
			/** \brief */
      int _poutput[2];
			/** \brief */
      int _perror[2];
			/** \brief */
			jprocess_type_t _type;

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
			void ForkChild();

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
			Process();

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
			virtual jprocess_type_t GetType();

			/**
			 * \brief
			 *
			 */
			virtual pid_t GetPID();

			/**
			 * \brief
			 *
			 */
			virtual pid_t GetParentPID();

			/**
			 * \brief 
			 *
			 */
			virtual jio::InputStream * GetInputStream();
			
			/**
			 * \brief 
			 *
			 */
			virtual jio::OutputStream * GetOutputStream();
			
			/**
			 * \brief 
			 *
			 */
			virtual jio::InputStream * GetErrorStream();
			
			/**
			 * \brief
			 *
			 */
			virtual void Start();
		
			/**
			 * \brief
			 *
			 */
			virtual bool IsRunning();

			/**
			 * \brief
			 *
			 */
			virtual void Release();

			/**
			 * \brief
			 *
			 */
			virtual void WaitProcess();

};

}

#endif
