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
#ifndef J_SOCKET6_H
#define J_SOCKET6_H

#include "jinetaddress.h"
#include "jserversocket6.h"
#include "jsocketoptions.h"
#include "jsocketinputstream.h"
#include "jsocketoutputstream.h"
#include "jconnection.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdint.h>

namespace jsocket {

class ServerSocket6;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class Socket6 : public jsocket::Connection{

	friend class ServerSocket6; //Socket * ServerSocket6::Accept();

   private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief */
		SocketInputStream *_is;
		/** \brief */
		SocketOutputStream *_os;
		/** \brief */
		InetAddress *_address;
		/** \brief Bytes sent. */
		int64_t _sent_bytes;
		/** \brief Bytes received. */
		int64_t _receive_bytes;
		/** \brief */
		int _timeout;
#ifdef _WIN32
#else
		/** \brief */
		struct sockaddr_in6 _lsock;
		/** \brief */
		struct sockaddr_in6 _server_sock;
#endif

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
		Socket6(jsocket_t handler_, struct sockaddr_in6 server_, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);
	
	 public:
		/**
		 * \brief Constructor.
		 *
		 */
		Socket6(InetAddress *addr_, int port_, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);

		/**
		 * \brief Constructor.
		 *
		 */
		Socket6(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);

		/**
		 * \brief
		 *
		 */
		Socket6(std::string host_, int port_, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);

		/**
		 * \brief Constructor.
		 *
		 */
		Socket6(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_ = 0, int rbuf_ = 4096, int wbuf_ = 4096);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Socket6();

		/**
		 * \brief
		 *
		 */
		virtual jsocket_t GetHandler();

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
