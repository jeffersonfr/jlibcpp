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
#ifndef J_PROCESSOUTPUTSTREAM_H_
#define J_PROCESSOUTPUTSTREAM_H_

#include "jinputstream.h"
#include "joutputstream.h"

#include <stdint.h>

namespace jshared {

class Process;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class ProcessOutputStream : public jio::OutputStream {

	friend class Process;

	private:
#ifdef _WIN32
		HANDLE _fd;
#else
		int _fd;
#endif

	private:
		/**
		 * \brief
		 *
		 */
#ifdef _WIN32
		ProcessOutputStream(HANDLE fd);
#else
		ProcessOutputStream(int fd);
#endif
		
	public:
		/**
		 * \brief
		 *
		 */
		virtual ~ProcessOutputStream();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsEmpty();
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t Available();
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t GetSize();
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t Write(int64_t b);
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t Write(const char *data, int64_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t Write(std::string);
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t Flush();
		
		/**
		 * \brief
		 *
		 */
		virtual void Seek(int64_t index);
		
		/**
		 * \brief
		 *
		 */
		virtual void Close();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsClosed();
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t GetSentBytes();

};

}

#endif
