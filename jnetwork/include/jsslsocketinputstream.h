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
#ifndef J_SSLSOCKETINPUTSTREAM_H
#define J_SSLSOCKETINPUTSTREAM_H

#include "jio/jinputstream.h"
#include "jnetwork/jconnection.h"

#include <openssl/ssl.h>

namespace jnetwork {

/**
 * \brief SSLSocketInputStream.
 *
 * \author Jeff Ferr
 */
class SSLSocketInputStream : public jio::InputStream {

    private:
			/** \brief */
			Connection *_connection;
			/** \brief */
			SSL *_ssl;
			/** \brief */
			char *_buffer;
			/** \brief */
			int64_t _buffer_length;
			/** \brief */
			int64_t _current_index; 
			/** \brief */
			int64_t _end_index;
			/** \brief */
			int64_t _receive_bytes;
			/** \brief */
			bool _stream;

		public:
			/**
			 * \brief Construtor.
			 *
			 */
			SSLSocketInputStream(Connection *connection_, void *ssl, int64_t size_ = SOCK_RD_BUFFER_SIZE);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~SSLSocketInputStream();

			/**
			 * \brief jio::InputStream
			 *
			 */
			virtual bool IsEmpty();

			/**
			 * \brief jio::InputStream
			 *
			 */
			virtual int64_t Available();

			/**
			 * \brief jio::InputStream
			 *
			 */
			virtual int64_t GetSize();

			/**
			 * \brief jio::InputStream
			 *
			 */
			virtual int64_t GetPosition();

			/**
			 * \brief jio::InputStream
			 *
			 */
			virtual void Skip(int64_t skip);

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
			 * \brief jio::InputStream
			 *
			 */
			virtual bool IsClosed();

			/**
			 * \brief
			 *
			 * \return the number of bytes received, or 0 if the peer has shutdown.
			 *
			 * \exception SocketException an error occurred.
			 * 
			 */
			virtual int64_t Read();

			/**
			 * \brief
			 *
			 * \return the number of bytes received, or 0 if the peer has shutdown.
			 *
			 * \exception SocketException an error occurred.
			 *
			 */
			virtual int64_t Read(char *data_, int64_t data_length_);

			/**
			 * \brief
			 *
			 */
			virtual int64_t GetReadedBytes();

};

}

#endif
