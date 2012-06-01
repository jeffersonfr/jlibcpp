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
 a***************************************************************************/
#ifndef J_CONSOLE_H
#define J_CONSOLE_H

#include "jobject.h"

#include <string>

#include <stdint.h>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class System{

	public:

		/**
		 * \brief
		 *
		 */
		virtual ~System();

		/**
		 * \brief
		 *
		 */
		static void Beep(int freq, int delay);
		
		/**
		 * \brief
		 *
		 */
		static std::string GetCurrentUserName();
		
		/**
		 * \brief
		 *
		 */
		static void Exit(int i);

		/**
		 * \brief
		 *
		 */
		static void Abort();

		/**
		 * \brief
		 *
		 */
		static int GetUserID();
		
		/**
		 * \brief
		 *
		 */
		static int GetProcessID();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetResourceDirectory();

		/**
		 * \brief
		 *
		 */
		static std::string GetHomeDirectory();

		/**
		 * \brief
		 *
		 */
		static std::string GetCurrentDirectory();

		/**
		 * \brief
		 *
		 */
		static std::string GetEnviromentVariable(std::string key_, std::string default_);
		
		/**
		 * \brief
		 *
		 */
		static void UnsetEnviromentVariable(std::string key_);
			
		/**
		 * \brief
		 *
		 */
		static bool SetEnviromentVariable(std::string key_, std::string value_, bool overwrite_ = true);

		/**
		 * \brief
		 *
		 */
		static int GetLastErrorCode();
		
		/**
		 * \brief Error message according with GetLastErrorCode() function.
		 *
		 */
		static std::string GetLastErrorMessage();
		
		/**
		 * \brief
		 *
		 */
		static void EnableKeyboardBuffer(bool b);

		/**
		 * \brief
		 *
		 */
		static void EnableEcho(bool b);

		/**
		 * \brief
		 *
		 */
		static int KbHit(void);
		
		/**
		 * \brief
		 *
		 */
		static int Getch(void);

		/**
		 * \brief
		 *
		 */
		static void ChangeWorkingDirectory(std::string dir);
		
		/**
		 * \brief
		 *
		 */
		static std::string GetProcessName();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetUserName();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetHostName();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetHostArchitecture();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetOSName();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetOSVersion();
		
		/**
		 * \brief
		 *
		 */
		static std::string GetTempDirectory();
		
		/**
		 * \brief
		 *
		 */
		static uint64_t GetDiskFreeSpace();
		
		/**
		 * \brief
		 *
		 */
		static int GetProcessorCount();
		
		/**
		 * \brief Reset the system.
		 *
		 */
		static int ResetSystem();
		
		/**
		 * \brief 
		 *
		 */
		static void Logout();
		
		/**
		 * \brief Shutdown the system.
		 *
		 */
		static int ShutdownSystem();
		
		/**
		 * \brief Reset the current program.
		 *
		 */
		static void ResetProgram(std::string program, char **argv, char **envp);
		
};

}

#endif
