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
#ifndef J_PIPESTREAM_H
#define j_PIPESTREAM_H

#include "jinputstream.h"
#include "joutputstream.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#endif

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class PipeStream : public jio::InputStream, jio::OutputStream{

	private:
#ifdef _WIN32
		/** \brief */
		HANDLE _fdr;
		/** \brief */
		HANDLE _fdw;
#else
		/** \brief */
		int _fds[2];
#endif
		/** \brief */
		bool _is_open;

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		PipeStream(int = 4096);
		
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~PipeStream();

		/**
		 * \brief
		 *
		 */
		long Read(char *data_, long length_);

		/**
		 * \brief
		 *
		 */
		long Write(const char *data_, long length_);
		
		/**
		 * \brief
		 *
		 */
		void Close();
	
};

}

#endif
