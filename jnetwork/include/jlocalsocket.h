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
#ifndef J_LOCALSOCKET_H
#define J_LOCALSOCKET_H

#include "jnetwork/jsocket.h"

#include <sys/socket.h>
#include <sys/un.h>

namespace jnetwork {

class LocalServerSocket;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class LocalSocket : public jnetwork::Connection {

	friend class LocalServerSocket;

  private:
		/** \brief */
		struct sockaddr_un _address;
		/** \brief */
		SocketInputStream *_is;
		/** \brief */
		SocketOutputStream *_os;
		/** \brief */
		std::string _file;
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
		 * \brief Connect the socket.
		 *
		 */
		void ConnectSocket();

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
		LocalSocket(int handler_, std::string file_, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

	 public:
		/**
		 * \brief
		 *
		 */
		LocalSocket(std::string file, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~LocalSocket();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetLocalFile();

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

};

}

#endif
