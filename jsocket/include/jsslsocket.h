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
#ifndef J_SSLSOCKET_H
#define J_SSLSOCKET_H

#include "jinetaddress.h"
#include "jserversocket.h"
#include "jsocketoptions.h"
#include "jsslsocketinputstream.h"
#include "jsslsocketoutputstream.h"
#include "jconnection.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif

#include <stdint.h>

/**
 *  It's VERY important that these types really have the right sizes!
 *
 */
#define COMPILE_TIME_ASSERT(name, x)	typedef int _dummy_ ## name[(x) * 2 - 1]
COMPILE_TIME_ASSERT(uint8, sizeof(uint8_t) == 1);
COMPILE_TIME_ASSERT(sint8, sizeof(int8_t) == 1);
COMPILE_TIME_ASSERT(uint16, sizeof(uint16_t) == 2);
COMPILE_TIME_ASSERT(sint16, sizeof(int16_t) == 2);
COMPILE_TIME_ASSERT(uint32, sizeof(uint32_t) == 4);
COMPILE_TIME_ASSERT(sint32, sizeof(int32_t) == 4);
#undef COMPILE_TIME_ASSERT

/**
 * Default values for RSA key generation
 *
 */
#define RSA_KEYSIZE 512
#define RSA_KEYEXP RSA_F4 /* 65537 */

namespace jsocket {

struct peer_cert_info_t {
	// Issuer name
	std::string commonName;             // CN
	std::string countryName;            // C
	std::string localityName;           // L
	std::string stateOrProvinceName;    // ST
	std::string organizationName;       // O
	std::string organizationalUnitName; // OU
	std::string title;                  // T
	std::string initials;               // I
	std::string givenName;              // G
	std::string surname;                // S
	std::string description;            // D
	std::string uniqueIdentifier;       // UID
	std::string emailAddress;           // Email

	// Expire dates
	std::string notBefore;
	std::string notAfter;

	// Misc. data
	long serialNumber;
	long version;
	std::string sgnAlgorithm;
	std::string keyAlgorithm;
	int keySize;
};

class ServerSocket;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SSLSocket : public jsocket::Connection{

	friend class SSLServerSocket; //Socket * ServerSocket::Accept();

	private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief */
		SSLSocketInputStream *_is;
		/** \brief */
		SSLSocketOutputStream *_os;
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
		sockaddr_in _lsock;
		/** \brief */
		sockaddr_in _server_sock;
		/** \brief */
		InetAddress *_local;
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
		void InitStreams(int rbuf, int wbuf);

		/**
		 * \brief
		 *
		 */
		bool Accept();
		
#ifdef _WIN32
#else
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
		 * Helper function: converts from X509 format to ASCII PEM format
		 *
		 */
		int GetCertPEM(X509 *cert, std::string *pem);
#endif

	private:
		/**
		 * \brief Constructor.
		 *
		 */
		SSLSocket(int handler_, struct sockaddr_in server_, int keysize = RSA_KEYSIZE, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

	public:
		/**
		 * \brief Constructor.
		 *
		 */
		SSLSocket(InetAddress *addr_, int port_, int keysize = RSA_KEYSIZE, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Constructor.
		 *
		 */
		SSLSocket(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int keysize = RSA_KEYSIZE, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief
		 *
		 */
		SSLSocket(std::string host_, int port_, int keysize = RSA_KEYSIZE, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Constructor.
		 *
		 */
		SSLSocket(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int keysize = RSA_KEYSIZE, int timeout_ = 0, int rbuf_ = SOCK_WR_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SSLSocket();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		static void InitializeSSL();

		/**
		 * \brief Accept a new socket.
		 *
		 */
		static void ReleaseSSL();

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
		virtual InetAddress * GetInetAddress();

		/**
		 * \brief Get the local port.
		 *
		 */
		virtual int GetLocalPort();

		/**
		 * \brief Get the port.
		 *
		 */
		virtual int GetPort();

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
		virtual SocketOptions * GetSocketOptions();

		/** 
		 * Cert files (if not set, a temporary RSA session cert will be created if needed)
		 * 
		 * PEM format
		 */
		bool UseCert(const char *cert_file, const char *private_key_file);

		/**
		 * As use_cert() but also give pasord for private_key_file (or get OpenSSL's standard prompt each time)
		 *
		 */
		bool UseCertPassword(const char *cert_file, const char *private_key_file, std::string pasd);

		/**
		 * Or specify a pasord callback given to OpenSSL that hands back the pasord to be used 
		 * during decryption. On invocation a pointer to userdata is provided. The pem_pasd_cb 
		 * must write the pasord into the provided buffer buf which is of size size. The actual 
		 * length of the pasord must be returned to the calling function. rwflag indicates
		 * whether the callback is used for reading/decryption (rwflag=0) or writing/encryption 
		 * (rwflag=1).
		 *
		 * See man SSL_CTX_set_default_pasd_cb(3) for more information.
		 */
		bool UseCertCallback(const char *cert_file, const char *private_key_file, int pasd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata = NULL);

		/**
		 *  Use Diffie-Hellman key exchange?
		 *
		 *  See man SSL_CTX_set_tmp_dh_callback(3) for more information.
		 */
		bool UseDHFile(const char *dh_file);

		/**
		 *  Should the peer certificate be verified ? The arguments specifies the locations at which CA
		 *  certificates for verification purposes are located. The certificates available via ca_file 
		 *  and ca_dir are trusted.
		 *
		 *   See man SSL_CTX_load_verify_locations(3) for format information.
		 */
		bool UseVerification(const char *ca_file, const char *ca_dir);

		/**
		 *  Get information about peer certificate. Should be called after connect() or accept() when 
		 *  using verification
		 *
		 */
		bool GetPeerCertInfo(peer_cert_info_t *info);

		/**
		 *  Get peer certificate in PEM (ASCII) format. Should be called after connect() or accept() 
		 *  when using verification.
		 *
		 *  Returns the length of pem or -1 on errors
		 */
		int GetPeerCertPEM(std::string *pem);

		/**
		 * \brief
		 *
		 */
		virtual std::string what();

};

}

#endif
