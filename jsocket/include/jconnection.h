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
#ifndef J_CONNECTION_H
#define J_CONNECTION_H

#include "jobject.h"
#include "jinputstream.h"
#include "joutputstream.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

namespace jsocket {

/**
 * \brief
 * 
 */
enum jconnection_type_t {
	TCP_SOCKET,
	UDP_SOCKET,
	MCAST_SOCKET,
	RTP_SOCKET,
	RAW_SOCKET,
	UNKNOWN_SOCKET
};

/**
 * \brief Connection.
 *
 * \author Jeff Ferr
 */
class Connection : public virtual jcommon::Object{

	friend class DatagramSocket;
	friend class Socket;
	friend class RTPSocket;
	friend class MulticastSocket;
	friend class RawSocket;
	
	protected:
	/** \brief */
	jconnection_type_t _type;
	/** \brief */
	bool _is_closed;

	/**
	 * \brief
	 *
	 */
	Connection(jconnection_type_t type);

	public:
	/**
	 * \brief
	 *
	 */
	virtual jconnection_type_t GetType();

	/**
	 * \brief Send bytes to a destination.
	 *
	 */
	virtual int Send(const char *b_, int size_, bool block_ = true) = 0;

	/**
	 * \brief Send bytes to a destination.
	 *
	 */
	virtual int Send(const char *b_, int size_, int time_) = 0;

	/**
	 * \brief Receive bytes from a source.
	 *
	 * \return the number of bytes received, or 0 if the peer has shutdown.
	 *
	 * \exception SocketException an error occurred.
	 *
	 */
	virtual int Receive(char *data_, int data_length_, bool block_ = true) = 0;

	/**
	 * \brief Receive bytes from a source.
	 *
	 * \return the number of bytes received, or 0 if the peer has shutdown.
	 *
	 * \exception SocketException an error occurred.
	 *
	 */
	virtual int Receive(char *data_, int data_length_, int time_) = 0;

	/**
	 * \brief 
	 *
	 */
	virtual bool IsClosed();

	/**
	 * \brief Close the socket.
	 *
	 */
	virtual void Close() = 0;

	/**
	 * \brief 
	 *
	 */
#ifdef _WIN32
		virtual SOCKET GetHandler() = 0;
#else
		virtual int GetHandler() = 0;
#endif
        
	/**
    * \brief
    *
    */
	virtual jio::InputStream * GetInputStream() = 0;

	/**
	 * \brief
	 *
	 */
	virtual jio::OutputStream * GetOutputStream() = 0;

	/**
	 * \brief Get the bytes sent to a destination.
	 *
	 */
	virtual int64_t GetSentBytes() = 0;

	/**
	 * \brief Get de bytes received from a source.
	 *
	 */
	virtual int64_t GetReadedBytes() = 0;


};

}

#endif
