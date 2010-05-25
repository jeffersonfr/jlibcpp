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
#ifndef J_MD5_H
#define J_MD5_H

#include "jobject.h"

#include <string>

#include <stdint.h>

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation. */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}

#define MD5_LEN_BIN 16
#define MD5_LEN_STR 32

namespace jmath {

enum md5_rc_t {
    MD5_RC_OK  = 0,
    MD5_RC_ARG = 1,
    MD5_RC_MEM = 2
};

struct MD5_CTX {
  uint32_t state[4];		// state (ABCD)
  uint32_t count[2];		// number of bits, modulo 2^64 (lsb first)
  uint8_t buffer[64];	// input buffer
};

/**
 * \brief This is a RFC 1321 compliant Message Digest 5 (MD5) algorithm
 * implementation. It is directly derived from the RSA code published in
 * RFC 1321 with just the following functionality preserving changes:
 * - converted function definitions from K&R to ANSI C
 * - included contents of the "global.h" and "md5.h" headers
 * - moved the SXX defines into the MD5Transform function
 * - replaced MD5_memcpy() with memcpy(3) and MD5_memset() with memset(3)
 * - renamed "index" variables to "idx" to avoid namespace conflicts
 * - reformatted C style to conform with OSSP C style
 * - added own OSSP style frontend API
 * 
 * \author Jeff Ferr
 */
class MD5 : public virtual jcommon::Object{

	private:

	public:
		/*
		 * \brief
		 *
		 */
		MD5();
		
		/*
		 * \brief
		 *
		 */
		virtual ~MD5();
	
		/* 
		 * \brief MD5 initialization. Begins an MD5 operation, writing a new context. 
		 *
		 */
		static void MD5Init(MD5_CTX *context);

		/* 
		 * \brief MD5 block update operation. Continues an MD5 message-digest
		 * operation, processing another message block, and updating the context. 
		 *
		 */
		static void MD5Update(MD5_CTX *context, uint8_t *input, unsigned int inputLen);

		/* 
		 * \brief MD5 finalization. Ends an MD5 message-digest operation, writing the
		 * the message digest and zeroizing the context. 
		 *
		 */
		static void MD5Final(MD5_CTX *context, uint8_t digest[]);

		/* 
		 * \brief MD5 basic transformation. Transforms state based on block. 
		 *
		 */
		static void MD5Transform(uint32_t state[], uint8_t block[]);

		/* 
		 * \brief Encodes input (uint32_t) into output (uint8_t).
		 * Assumes len is a multiple of 4. 
		 *
		 */
		static void Encode(uint8_t *output, uint32_t *input, unsigned int len);

		/* 
		 * \brief Decodes input (uint8_t) into output (uint32_t).
		 * Assumes len is a multiple of 4. 
		 *
		 */
		static void Decode(uint32_t *output, uint8_t *input, unsigned int len);


		/*
		 * \brief
		 *
		 */
		virtual std::string what();
};

}

#endif
