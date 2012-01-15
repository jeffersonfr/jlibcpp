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
#ifndef J_DATAGRAMSOCKET6_H
#define J_DATAGRAMSOCKET6_H

#include "jinetaddress6.h"
#include "jsocketoptions.h"
#include "jsocketinputstream.h"
#include "jsocketoutputstream.h"
#include "jconnection.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <stdint.h>

namespace jsocket {

/**
 * \brief Socket UDP.
 *
 * \author Jeff Ferr
 */
class DatagramSocket6 : public jsocket::Connection{

	private:
		/** \brief Use to bind the socket in a free port. */
		static int _used_port;

		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief Local socket. */
		struct sockaddr_in6 _lsock;
		/** \brief Server socket UDP. */
		struct sockaddr_in6 _server_sock;
		/** \brief Local inetaddress. */
		InetAddress6 *_local;
		/** \brief Remote inetaddress. */
		InetAddress *_address;
		/** \brief Input stream. */
		SocketInputStream *_is;
		/** \brief Output stream. */
		SocketOutputStream *_os;
		/** \brief Bytes sent. */
		int64_t _sent_bytes;
		/** \brief Bytes received. */
		int64_t _receive_bytes;
		/** \brief Connect or not ? */
		bool _stream;
		/** \brief */
		int _timeout;

		/**
		 * \brief Create a new socket.
		 *
		 */
		void CreateSocket();

		/**
		 * \brief Bind socket.
		 *
		 */
		void BindSocket(InetAddress *addr_, int local_port_);

		/**
		 * \brief Connect socket.
		 *
		 */
		void ConnectSocket(InetAddress *addr_, int port_);

		/**
		 * \brief Init the stream.
		 *
		 */
		void InitStream(int rbuf_, int wbuf_);

	public:
		/**
		 * \brief Construtor UDP client.
		 *
		 */
		DatagramSocket6(std::string addr_, int port_, bool stream_ = false, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);

		/**
		 * \brief Construtor UDP server.
		 *
		 */
		DatagramSocket6(int port_, bool stream_ = false, int timeout_ = 0, int rbuf_ = 65535, int wbuf_ = 4096);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~DatagramSocket6();

		/**
		 * \brief 
		 *
		 */
		virtual jsocket_t GetHandler();

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
		 * \brief Read data from a source.
		 *
		 */
		virtual int Receive(char *data_, int size_, bool block_ = true);

		/**
		 * \brief Read data from a source.
		 *
		 */
		virtual int Receive(char *data_, int size_, int time_);

		/**
		 * \brief Write data to a source.
		 *
		 */
		virtual int Send(const char *data_, int size_, bool block_ = true);

		/**
		 * \brief Write data to a source.
		 *
		 */
		virtual int Send(const char *data_, int size_, int time_);

		/**
		 * \brief Close the socket.
		 *
		 */
		virtual void Close();        

		/**
		 * \brief Get InetAddress.
		 *
		 */
		InetAddress * GetInetAddress();

		/**
		 * \brief Get the local port.
		 *
		 */
		int GetLocalPort();

		/**
		 * \brief Get port.
		 *
		 */
		int GetPort();

		/**
		 * \brief Get sent bytes to destination.
		 *
		 */
		virtual int64_t GetSentBytes();

		/**
		 * \brief Get received bytes from a source.
		 *
		 */
		virtual int64_t GetReadedBytes();

		/**
		 * \brief Get a object SocketOptions.
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
