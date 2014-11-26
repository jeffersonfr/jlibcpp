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
#ifndef J_NAMEDPIPE_H
#define J_NAMEDPIPE_H

#include "jobject.h"

#include <iostream>

#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace jshared {

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class NamedPipe : public virtual jcommon::Object{

	private:
		/** \brief */
		std::string _name;
#ifdef _WIN32
		/** \brief Socket handler. */
		HANDLE _fd;
#else
		/** \brief */
		int _fd;
#endif
		/** \brief */
		bool _is_closed;

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		NamedPipe(std::string name, int mode);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~NamedPipe();

		/**
		 * \brief
		 *
		 */
		virtual int Send(const char *data_, int data_length);

		/**
		 * \brief
		 *
		 */
		virtual int Receive(char *data_, int data_length_);

		/**
		 * \brief
		 *
		 */
		virtual bool IsClosed();

		/**
		 * \brief Close.
		 *
		 */
		virtual void Close();

};

}

#endif
