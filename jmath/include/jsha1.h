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
#ifndef J_SHA1_H
#define J_SHA1_H

#include "jobject.h"

#include <string>

#include <stdint.h>

#define SHA1HashSize 20

/* Define the SHA1 circular left shift macro */
#define SHA1CircularShift(bits,word) \
    (((word) << (bits)) | ((word) >> (32-(bits))))

#define SHA1_LEN_BIN 20
#define SHA1_LEN_STR 40

namespace jmath {

enum jsha1_rc_t {
    SHA1_RC_OK  = 0,
    SHA1_RC_ARG = 1,
    SHA1_RC_MEM = 2,
    SHA1_RC_INT = 3
};

enum {
    shaSuccess = 0,
    shaNull,				// null pointer parameter
    shaStateError		// called Input after Result
};

/* This structure will hold context information for the SHA-1 hashing operation */
struct SHA1Context {
    uint32_t Intermediate_Hash[SHA1HashSize/4];	// Message Digest
    uint32_t Length_Low;						// Message length in bits
    uint32_t Length_High;						// Message length in bits
    uint32_t Message_Block_Index;		// Index into message block array
    uint8_t Message_Block[64];			// 512-bit message blocks
    int Computed;										// Is the digest computed?
    int Corrupted;									// Is the message digest corrupted?
};

/**
 * \brief This implements the Secure Hashing Algorithm 1 as defined in
 *  FIPS PUB 180-1 published April 17, 1995.
 *
 *  The SHA-1, produces a 160-bit message digest for a given data
 *  stream. It should take about 2**n steps to find a message with the
 *  same digest as a given message and 2**(n/2) to find any two messages
 *  with the same digest, when n is the digest size in bits. Therefore,
 *  this algorithm can serve as a means of providing a "fingerprint" for
 *  a message.
 *
 *  Caveats: SHA-1 is designed to work with messages less than 2^64 bits
 *  long. Although SHA-1 allows a message digest to be generated for
 *  messages of any number of bits less than 2^64, this implementation
 *  only works with messages with a length that is a multiple of the
 *  size of an 8-bit character.
 * 
 * \author Jeff Ferr
 */
class SHA1 : public virtual jcommon::Object{

	private:
		
	public:
		/*
		 * \brief
		 *
		 */
		SHA1();

		/*
		 * \brief
		 *
		 */
		virtual ~SHA1();

		/*
		 * \brief This function will initialize the SHA1Context in preparation for
		 *  computing a new SHA1 message digest.
		 *
		 */
		static int SHA1Reset  (SHA1Context *);

		/*
		 * \brief This function accepts an array of octets as the next portion of the message.
		 *
		 */
		static int SHA1Input  (SHA1Context *, const uint8_t *, unsigned int);
		
		/*
		 * \brief This function will return the 160-bit message digest into the
		 * Message_Digest array provided by the caller. NOTE: The first octet
		 * of hash is stored in the 0th element, the last octet of hash in the
		 * 19th element.
		 */
		static int SHA1Result (SHA1Context *, uint8_t Message_Digest[]);

		/*
		 * \brief According to the standard, the message must be padded to an even
		 * 512 bits. The first padding bit must be a '1'. The last 64 bits
		 * represent the length of the original message. All bits in between
		 * should be 0. This function will pad the message according to those
		 * rules by filling the Message_Block array accordingly. It will also
		 * call the ProcessMessageBlock function provided appropriately. When
		 * it returns, it can be assumed that the message digest has been computed.
		 */
		static void SHA1PadMessage (SHA1Context *);

		/*
		 * \brief This function will process the next 512 bits of the message stored
		 * in the Message_Block array. NOTICE: Many of the variable names in
		 * this code, especially the single character names, were used because
		 * those were the names used in the publication.
		 */
		static void SHA1ProcessMessageBlock(SHA1Context *);

		/*
		 * \brief
		 *
		 */
		virtual std::string what();

};

}

#endif
