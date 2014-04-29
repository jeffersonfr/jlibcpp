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
#ifndef J_SSLSERVERSOCKET6_H
#define J_SSLSERVERSOCKET6_H

#include "jsslsocket6.h"
#include "jsslcontext.h"
#include "jinetaddress6.h"
#include "jobject.h"

#include <sys/socket.h>

#include <stdint.h>

namespace jsocket {

class SSLSocket;

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class SSLServerSocket6 : public virtual jcommon::Object{

	private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief */
		bool _is_closed;
		/** \brief Local socket. */
		sockaddr_in6 _lsock;
		/** \brief Remote socket. */
		sockaddr_in6 _rsock;
		/** \brief */
		InetAddress6 *_local;
		/** \brief */
		SSLContext *_ctx;

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
		SSLServerSocket6(SSLContext *ctx, int port, int backlog = 5, InetAddress * = NULL);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~SSLServerSocket6();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		virtual SSLSocket6 * Accept();

		/**
		 * \brief
		 *
		 */
		virtual SSLContext * GetContext();

		/**
		 * \brief
		 *
		 */
		virtual InetAddress * GetInetAddress();

		/**
		 * \brief Get the local port.
		 *
		 */
		virtual int GetLocalPort();

		/**
		 * \brief Close the server socket.
		 *
		 */
		virtual void Close();

		/**
		 * \brief 
		 *
		 */
		virtual bool IsClosed();

};

}

#endif

