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

#include "jsslsocket.h"
#include "jinetaddress.h"
#include "jobject.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdint.h>

namespace jsocket {

class SSLSocket;

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class SSLServerSocket : public virtual jcommon::Object{

	private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief Local socket. */
		sockaddr_in _lsock;
		/** \brief Remote socket. */
		sockaddr_in _rsock;
		/** \brief */
		InetAddress *_local;
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

		/**
		 * Create new CTX if none is available
		 *
		 */
		virtual bool CheckContext();

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		virtual bool CheckCert();

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		RSA * GenerateRSAKey(int len, int exp = RSA_KEYEXP);

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		EVP_PKEY * GeneratePKey(RSA *rsakey);

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		X509 * BuildCertificate(const char *name, const char *organization, const char *country, EVP_PKEY *key);

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		bool UseCertCallback(const char *cert_file, const char *private_key_file, int passwd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata);

		/** \brief SSL data */
		SSL_CTX *ctx;
		/** \brief SSL data */
		SSL *ssl;
		/** \brief Indicate CERT loaded or created */
		bool have_cert; 
		/** \brief keysize argument from constructor */
		int rsa_keysize;
		/** \brief userdata */
		char *ud;

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		SSLServerSocket(int port, int backlog = 5, int keysize = RSA_KEYSIZE, InetAddress * = NULL);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~SSLServerSocket();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		SSLSocket * Accept();

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
		 * \brief Close the server socket.
		 *
		 */
		void Close();

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		bool UseCertPassword(const char *cert_file, const char *private_key_file, std::string password);

		/**
		 *  Create temp cert if no other is loaded
		 *
		 */
		bool UseDHFile(const char *dh_file);

		/**
		 * \brief 
		 *
		 */
		virtual bool IsClosed();

};

}

#endif

