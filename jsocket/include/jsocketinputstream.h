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
#ifndef J_SOCKET_INPUT_STREAM_H
#define J_SOCKET_INPUT_STREAM_H

#include "jobject.h"
#include "jinputstream.h"
#include "jconnection.h"

#include <stdexcept>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace jsocket {

/**
 * \brief SocketInputStream.
 *
 * \author Jeff Ferr
 */
class SocketInputStream : public jio::InputStream{

    private:
#ifdef _WIN32
		SOCKET _fd;
#else
		int _fd;
#endif
       /** \brief */
        char *_buffer;
        /** \brief */
        int _buffer_length;
        /** \brief */
		int _current_index; 
        /** \brief */
		int _end_index;
        /** \brief */
        long long _receive_bytes;
        /** \brief */
		bool _stream;
        /** \brief */
		sockaddr_in _server_sock;
        /** \brief */
		bool *_is_closed;
        /** \brief */
		Connection *_connection;

    public:
        /**
        * \brief Construtor.
        *
        */
        SocketInputStream(Connection *conn_, bool *is_closed_, int size_ = 65535);
        
        /**
        * \brief Construtor.
        *
        */
        SocketInputStream(Connection *conn_, bool *is_closed_, sockaddr_in server_sock_, int size_ = 65535);
		
        /**
        * \brief Destrutor virtual.
        *
        */
        virtual ~SocketInputStream();

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual bool IsEmpty();

		/**
		 * \brief jio::InputStream
		 *
		 */
   		virtual long long Available();

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual long long GetSize();

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual long long GetPosition();

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual void Skip(long long skip);

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual void Reset();

		/**
		 * \brief jio::InputStream
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 * \return the number of bytes received, or 0 if the peer has shutdown.
		 *
		 * \exception SocketException an error occurred.
		 * 
		 */
		virtual int Read();

		/**
		 * \brief
		 *
		 * \return the number of bytes received, or 0 if the peer has shutdown.
		 *
		 * \exception SocketException an error occurred.
		 *
		 */
		virtual long long Read(char *data_, long long data_length_);

		/**
		 * \brief
		 *
		 */
		long long GetReceiveBytes();

};

}

#endif
