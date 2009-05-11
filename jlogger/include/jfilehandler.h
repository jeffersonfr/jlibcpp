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
#ifndef J_LOGGERFILEHANDLER_H
#define J_LOGGERFILEHANDLER_H

#include "jstreamhandler.h"
#include "jfile.h"
#include "jruntimeexception.h"
#include "jmutex.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#endif

namespace jlogger {

/**
 * \brief
 * 
 * \author Jeff Ferr
*/
class FileHandler : public jlogger::StreamHandler{

	private:
		/** \brief */
		jio::File *_file;
		/** \brief */
		struct stat _stat;
		/** \brief */
		jthread::Mutex _mutex;

	public:
		/**
		 * \brief
		 */
    	FileHandler(std::string filename_);

		/**
		 * \brief
		 */
		virtual ~FileHandler();

		/**
		 * \brief
		 *
		 */
		virtual void WriteRecord(LogRecord *);

};

}

#endif
