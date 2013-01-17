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
#ifndef J_SSLSOCKETOUTPUTSTREAM_H
#define J_SSLSOCKETOUTPUTSTREAM_H

#include "jobject.h"
#include "joutputstream.h"
#include "jconnection.h"

#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif

namespace jsocket {

/**
 * \brief SSLSocketOutputStream.
 *
 * \author Jeff Ferr
 */
class SSLSocketOutputStream : public jio::OutputStream{

    private:
#ifdef _WIN32
#else
			/** \brief */
			jsocket_t _fd;
			/** \brief */
			Connection *_connection;
			/** \brief */
			char *_buffer;
			/** \brief */
			int64_t _buffer_length;
			/** \brief */
			int64_t _current_index; 
			/** \brief */
			int64_t _sent_bytes;
			/** \brief */
			bool _stream;
			/** \brief */
			SSL *_ssl;
#endif

		public:
			/**
			 * \brief Construtor.
			 *
			 */
			SSLSocketOutputStream(Connection *conn_, void *ssl, int64_t size_ = 4096LL);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~SSLSocketOutputStream();

			/**
			 * \brief
			 *
			 */
			virtual bool IsEmpty();

			/**
			 * \brief
			 *
			 */
			virtual int64_t Available();

			/**
			 * \brief jio::OutputStream
			 *
			 */
			virtual int64_t GetSize();

			/**
			 * \brief jio::OutputStream
			 *
			 */
			virtual void Seek(int64_t index);

			/**
			 * \brief jio::OutputStream
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
			 */
			virtual int64_t Write(int64_t c_);

			/**
			 * \brief
			 *
			 */
			virtual int64_t Write(const char *data_, int64_t data_length_);

			/**
			 * \brief
			 *
			 */
			virtual int64_t GetAvailable();

			/**
			 * \brief
			 *
			 */
			virtual int64_t GetSentBytes();

			/**
			 * \brief
			 *
			 */
			virtual int64_t Flush();
};

}

#endif
