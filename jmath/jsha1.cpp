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
#include "Stdafx.h"
#include "jmathlib.h"

namespace jmath {

SHA1::SHA1():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::SHA1");
}

SHA1::~SHA1()
{
}

int SHA1::SHA1Reset(SHA1Context *context)
{
    if (context == NULL) {
        return shaNull;
		}

    context->Length_Low             = 0;
    context->Length_High            = 0;
    context->Message_Block_Index    = 0;

    context->Intermediate_Hash[0]   = 0x67452301;
    context->Intermediate_Hash[1]   = 0xEFCDAB89;
    context->Intermediate_Hash[2]   = 0x98BADCFE;
    context->Intermediate_Hash[3]   = 0x10325476;
    context->Intermediate_Hash[4]   = 0xC3D2E1F0;

    context->Computed   = 0;
    context->Corrupted  = 0;

    return shaSuccess;
}

int SHA1::SHA1Result(SHA1Context *context, uint8_t Message_Digest[])
{
    int i;

    if (context == NULL || Message_Digest == NULL)
        return shaNull;
    if (context->Corrupted)
        return context->Corrupted;

    if (!context->Computed) {
        SHA1PadMessage(context);
        for (i = 0; i < 64; i++) {
            /* message may be sensitive, clear it out */
            context->Message_Block[i] = (uint8_t)0;
        }
        context->Length_Low  = 0; /* and clear length */
        context->Length_High = 0;
        context->Computed    = 1;
    }
    for (i = 0; i < SHA1HashSize; i++)
        Message_Digest[i] = (uint8_t)(context->Intermediate_Hash[i>>2] >> (8 * (3 - (i & 0x03))));

    return shaSuccess;
}

int SHA1::SHA1Input(SHA1Context *context, const uint8_t *message_array, unsigned int length)
{
    if (length == 0)
        return shaSuccess;
    if (context == NULL || message_array == NULL)
        return shaNull;

    if (context->Computed) {
        context->Corrupted = shaStateError;
        return shaStateError;
    }
    if (context->Corrupted)
        return context->Corrupted;
    while (length-- && !context->Corrupted) {
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
        context->Length_Low += 8;
        if (context->Length_Low == 0) {
            context->Length_High++;
            if (context->Length_High == 0)
                context->Corrupted = 1; /* Message is too long */
        }
        if (context->Message_Block_Index == 64)
            SHA1ProcessMessageBlock(context);
        message_array++;
    }

    return shaSuccess;
}

void SHA1::SHA1ProcessMessageBlock(SHA1Context *context)
{
    const uint32_t K[] = {   /* Constants defined in SHA-1   */
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int            t;             /* Loop counter                */
    uint32_t  temp;          /* Temporary word value        */
    uint32_t  W[80];         /* Word sequence               */
    uint32_t  A, B, C, D, E; /* Word buffers                */

    /* Initialize the first 16 words in the array W */
    for (t = 0; t < 16; t++) {
        W[t]  = (uint32_t)(context->Message_Block[t * 4    ] << 24);
        W[t] |= (uint32_t)(context->Message_Block[t * 4 + 1] << 16);
        W[t] |= (uint32_t)(context->Message_Block[t * 4 + 2] <<  8);
        W[t] |= (uint32_t)(context->Message_Block[t * 4 + 3]      );
    }

    for (t = 16; t < 80; t++)
       W[t] = SHA1CircularShift(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for (t = 0; t < 20; t++) {
        temp =  SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    context->Intermediate_Hash[1] += B;
    context->Intermediate_Hash[2] += C;
    context->Intermediate_Hash[3] += D;
    context->Intermediate_Hash[4] += E;

    context->Message_Block_Index = 0;

    return;
}

void SHA1::SHA1PadMessage(SHA1Context *context)
{
    /* Check to see if the current message block is too small to hold
       the initial padding bits and length. If so, we will pad the block,
       process it, and then continue padding into a second block. */
    if (context->Message_Block_Index > 55) {
        context->Message_Block[context->Message_Block_Index++] = (uint8_t)0x80;
        while (context->Message_Block_Index < 64)
            context->Message_Block[context->Message_Block_Index++] = (uint8_t)0;
        SHA1ProcessMessageBlock(context);
        while(context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = (uint8_t)0;
    }
    else {
        context->Message_Block[context->Message_Block_Index++] = (uint8_t)0x80;
        while(context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = (uint8_t)0;
    }

    /* Store the message length as the last 8 octets */
    context->Message_Block[56] = (uint8_t)(context->Length_High >> 24);
    context->Message_Block[57] = (uint8_t)(context->Length_High >> 16);
    context->Message_Block[58] = (uint8_t)(context->Length_High >>  8);
    context->Message_Block[59] = (uint8_t)(context->Length_High      );
    context->Message_Block[60] = (uint8_t)(context->Length_Low  >> 24);
    context->Message_Block[61] = (uint8_t)(context->Length_Low  >> 16);
    context->Message_Block[62] = (uint8_t)(context->Length_Low  >>  8);
    context->Message_Block[63] = (uint8_t)(context->Length_Low       );

    SHA1ProcessMessageBlock(context);
    return;
}

std::string SHA1::what()
{
   return "";
}

}



