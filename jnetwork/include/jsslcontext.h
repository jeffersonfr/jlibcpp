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
#ifndef J_SSLCONTEXT_H
#define J_SSLCONTEXT_H

#include "jcommon/jobject.h"

#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/**
 *  It's VERY important that these types really have the right sizes!
 *
 */
#define COMPILE_TIME_ASSERT(name, x)  typedef int _dummy_ ## name[(x) * 2 - 1]
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
#define RSA_KEYEXP RSA_F4 // 65537 

namespace jnetwork {

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class SSLContext : public virtual jcommon::Object {

	private:
		/** \brief */
		SSL_CTX *_ctx;

	private:
		/**
		 * \brief
		 *
		 */
		RSA * GenerateRSAKey(int len, int exp = RSA_KEYEXP);

		/**
		 * \brief
		 *
		 */
		EVP_PKEY * GeneratePKey(RSA *rsakey);

		/**
		 * /brief Create temporary certificate.
		 *
		 */
		X509 * BuildCertificate(std::string name, std::string organization, std::string country, EVP_PKEY *key);

	private:
		/**
		 * \brief Constructor.
		 *
		 */
		SSLContext(SSL_METHOD *method);

		/**
		 * \brief Constructor.
		 *
		 */
		SSLContext(std::string ca_file, SSL_METHOD *method);

	public:
		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~SSLContext();

		/**
		 * \brief 
		 *
		 */
		static SSLContext * CreateServerContext(SSL_METHOD *method = NULL);

		/**
		 * \brief 
		 *
		 */
		static SSLContext * CreateClientContext(std::string ca_file, SSL_METHOD *method = NULL);

		/**
		 * \brief 
		 *
		 */
		virtual SSL_CTX * GetSSLContext();

		/**
		 * \brief 
		 *
		 */
		virtual bool SetCertificate(std::string cert_file, std::string pkey_file);

		/**
		 * \brief 
		 *
		 */
		virtual bool SetCertificate(std::string cert_file, std::string pkey_file, std::string password);

		/**
		 * \brief 
		 *
		 */
		virtual bool SetCertificate(std::string cert_file, std::string pkey_file, int passwd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata);

		/**
		 * \brief
		 *
		 */
		virtual bool VerifyRootAuthorityFile(std::string file);
		
		/**
		 * \brief
		 *
		 */
		virtual bool VerifyRootAuthorityLocation(std::string file);

		/**
		 * \brief 
		 *
		 */
		virtual void SetRootAuthorities(std::string file, int depth = 1);

		/**
		 * \brief 
		 *
		 */
		virtual void SetDHFile(std::string file);

		/**
		 * \brief 
		 *
		 */
		virtual void SetCertificateChain(std::string cert_file);

};

}

#endif

