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
#ifndef J_MULTICASTSOCKET6_H
#define J_MULTICASTSOCKET6_H

#include "jinetaddress6.h"
#include "jsocketoptions.h"
#include "jsocketinputstream.h"
#include "jsocketoutputstream.h"
#include "jconnection.h"

#include "jobject.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <sys/types.h>
#include <stdint.h>

namespace jsocket {

/**
 * \brief MulticastSocket.
 *
 * \author Jeff Ferr
 */
class MulticastSocket6 : public jsocket::Connection{

	private:
		/** \brief Use to bind the socket in a free port */
		static int _used_port;
        
		/** \brief */
		std::vector<std::string> _groups;
		/** \brief Socket handler. */
		jsocket_t _fds, _fdr;
		/** \brief Input stream */
		SocketInputStream *_is;
		/** \brief Output stream */
		SocketOutputStream *_os;
		/** \brief */
		int64_t _sent_bytes;
		/** \brief */
		int64_t _receive_bytes;
#ifdef _WIN32
#else
		/** \brief Local socket */
		struct sockaddr_in6 _sock_s;
		/** \brief Local socket */
		struct sockaddr_in6 _sock_r;
#endif

		/**
		 * \brief Create a new socket
		 *
		 */
		void CreateSocket();

		/**
		 * \brief Bind socket
		 *
		 */
		void BindSocket(InetAddress *addr_, int local_port_);

		/**
		 * \brief Connect socket
		 *
		 */
		void ConnectSocket(InetAddress *addr_, int port_);

		/**
		 * \brief
		 *
		 */
		void InitStream(int rbuf_, int wbuf_);

	public:
		/**
		 * \brief 
		 *
		 */
		MulticastSocket6(std::string addr_, int port_, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~MulticastSocket6();

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
		 * \brief
		 *
		 */
		virtual int Receive(char *data_, int size_, bool block_ = true);

		/**
		 * \brief
		 *
		 */
		virtual int Receive(char *data_, int size_, int time_);

		/**
		 * \brief
		 *
		 */
		virtual int Send(const char *data_, int size_, bool block_ = true);

		/**
		 * \brief
		 *
		 */
		virtual int Send(const char *data_, int size_, int time_);

		/**
		 * \brief
		 *
		 */
		void Join(std::string group_);

		/**
		 * \brief
		 *
		 */
		void Join(InetAddress *group_);

		/**
		 * \brief
		 *
		 */
		void Leave(std::string group_);

		/**
		 * \brief
		 *
		 */
		void Leave(InetAddress *group_);

		/**
		 * \brief
		 *
		 */
		std::vector<std::string> & GetGroupList();

		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		int GetLocalPort();

		/**
		 * \brief
		 *
		 */
		virtual int64_t GetSentBytes();

		/**
		 * \brief
		 *
		 */
		virtual int64_t GetReadedBytes();

		/**
		 * \brief
		 *
		 */
		void SetMulticastTTL(char ttl_);

		/**
		 * \brief
		 *
		 */
		SocketOptions * GetSocketOptions();
		
		/**
		 * \brief
		 *
		 */
		SocketOptions * GetSocketOptionsExtension();

};

}

#endif
