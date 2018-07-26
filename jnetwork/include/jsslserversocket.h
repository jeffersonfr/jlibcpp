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
#ifndef J_SSLSERVERSOCKET_H
#define J_SSLSERVERSOCKET_H

#include "jnetwork/jsslsocket.h"
#include "jnetwork/jsslcontext.h"
#include "jnetwork/jinetaddress.h"

#include <sys/socket.h>

namespace jnetwork {

class SSLSocket;

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class SSLServerSocket : public virtual jcommon::Object {

	private:
		/** \brief Local socket. */
		sockaddr_in _lsock;
		/** \brief Remote socket. */
		sockaddr_in _rsock;
		/** \brief */
		InetAddress *_local;
		/** \brief */
		SSLContext *_ctx;
		/** \brief */
		int _fd;
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
		SSLServerSocket(SSLContext *ctx, int port, int backlog = 5, InetAddress * = NULL);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~SSLServerSocket();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		virtual SSLSocket * Accept();

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

