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
#ifndef J_SOCKET_H
#define J_SOCKET_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jserversocket.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <sys/socket.h>

namespace jnetwork {

class ServerSocket;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class Socket : public jnetwork::Connection {

	friend class ServerSocket; //Socket * ServerSocket::Accept();

   private:
		/** \brief */
		SocketInputStream *_is;
		/** \brief */
		SocketOutputStream *_os;
		/** \brief */
		struct sockaddr_in _lsock;
		/** \brief */
		struct sockaddr_in _server_sock;
		/** \brief */
		InetAddress *_local;
		/** \brief */
		InetAddress *_address;
		/** \brief Bytes sent. */
		int64_t _sent_bytes;
		/** \brief Bytes received. */
		int64_t _receive_bytes;
		/** \brief */
		int _timeout;

		/**
		 * \brief Create a new socket.
		 *
		 */
		void CreateSocket();

		/**
		 * \brief
		 *
		 */
		void BindSocket(InetAddress *, int);

		/**
		 * \brief Connect the socket.
		 *
		 */
		void ConnectSocket(InetAddress *, int);

		/**
		 * \brief
		 *
		 */
		void InitStreams(int64_t rbuf_, int64_t wbuf_);

	 protected:
		/**
		 * \brief Constructor.
		 *
		 */
		Socket(int fd_, struct sockaddr_in server_, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);
	
	 public:
		/**
		 * \brief Constructor.
		 *
		 */
		Socket(InetAddress *addr_, int port_, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Constructor.
		 *
		 */
		Socket(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief
		 *
		 */
		Socket(std::string host_, int port_, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Constructor.
		 *
		 */
		Socket(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_ = 0, int rbuf_ = SOCK_WR_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Socket();

		/**
		 * \brief Send bytes to a destination.
		 *
		 */
		virtual int Send(const char *b_, int size_, bool block_ = true);

		/**
		 * \brief Send bytes to a destination waiting a timeout.
		 *
		 */
		virtual int Send(const char *b_, int size_, int time_);

		/**
		 * \brief Receive bytes from a source.
		 *
		 * \return the number of bytes received, or 0 if the peer has shutdown (now throws).
		 *
		 * \exception SocketException an error occurred.
		 *
		 */
		virtual int Receive(char *data_, int data_length_, bool block_ = true);

		/**
		 * \brief Receive bytes from a source waiting a timeout.
		 *
		 */
		virtual int Receive(char *data_, int data_length_, int time_);

		/**
		 * \brief Close the socket.
		 *
		 */
		virtual void Close();

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
		InetAddress * GetInetAddress();

		/**
		 * \brief Get the local port.
		 *
		 */
		int GetLocalPort();

		/**
		 * \brief Get the port.
		 *
		 */
		int GetPort();

		/**
		 * \brief Get the bytes sent to a destination.
		 *
		 */
		virtual int64_t GetSentBytes();

		/**
		 * \brief Get de bytes received from a source.
		 *
		 */
		virtual int64_t GetReadedBytes();

		/**
		 * \brief Get the socket options.
		 *
		 */
		SocketOptions * GetSocketOptions();

		/**
		 * \brief
		 *
		 */
		virtual std::string what();

};

}

#endif
