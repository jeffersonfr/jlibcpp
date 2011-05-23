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
#ifndef J_SERVERSOCKET6_H
#define J_SERVERSOCKET6_H

#include "jconnection.h"
#include "jinetaddress.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdint.h>

namespace jsocket {

class Socket6;

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class ServerSocket6 : public virtual jcommon::Object{

	private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief Local socket. */
		struct sockaddr_in6 _lsock;
		/** \brief Remote socket. */
		struct sockaddr_in6 _rsock;
		/** \brief */
		InetAddress *_local;
		/** \brief */
		bool _is_closed;
        
		/**
		 * \brief
		 *
		 */
		void CreateSocket();

		/**
		 * \brief
		 *
		 */
		void BindSocket(InetAddress *, int);

		/**
		 * \brief
		 *
		 */
		void ListenSocket(int);

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		ServerSocket6(int port, int backlog = 5, InetAddress * = NULL);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~ServerSocket6();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		Socket6 * Accept();

		/**
		 * \brief
		 *
		 */
		InetAddress * GetInetAddress();

		/**
		 * \brief Get the local port.
		 *
		 */
		int GetLocalPort();

		/**
		 * \brief Close the server socket.
		 *
		 */
		void Close();

};

}

#endif