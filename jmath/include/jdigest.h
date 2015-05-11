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
 a***************************************************************************/
#ifndef J_DIGEST_H
#define J_DIGEST_H

#include "jobject.h"

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <openssl/opensslv.h>
// #include <openssl/md2.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
// #include <openssl/mdc2.h>

namespace jmath {

/**
 * \brief Digest algorithms.
 * 
 * \author Jeff Ferr
 */
class Digest : public virtual jcommon::Object{

	private:
		/** \brief */
		// MD2_CTX _md2_ctx;
		/** \brief */
		MD4_CTX _md4_ctx;
		/** \brief */
		MD5_CTX _md5_ctx;
		/** \brief */
		SHA_CTX _sha1_ctx;
		/** \brief */
		// SHA224_CTX _sha224_ctx;
		/** \brief */
		SHA256_CTX _sha256_ctx;
		/** \brief */
		// SHA384_CTX _sha384_ctx;
		/** \brief */
		SHA512_CTX _sha512_ctx;
		/** \brief */
		RIPEMD160_CTX _ripemd160_ctx;
		/** \brief */
		// MDC2_CTX _mdc2_ctx;
		/** \brief */
		std::string _method;
		/** \brief */
		int _digest_length;

	public:
		/*
		 * \brief
		 *
		 */
		Digest(std::string method);
		
		/*
		 * \brief
		 *
		 */
		virtual ~Digest();
	
		/*
		 * \brief
		 *
		 */
		virtual std::string GetMethod();

		/*
		 * \brief
		 *
		 */
		virtual jcommon::Object * Clone();

		/*
		 * \brief
		 *
		 */
		virtual void Reset();

		/*
		 * \brief
		 *
		 */
		virtual void Update(const char *data, int length);

		/*
		 * \brief
		 *
		 */
		virtual std::string GetResult(bool hex = true);

		/*
		 * \brief
		 *
		 */
		virtual std::string what();
};

}

#endif
