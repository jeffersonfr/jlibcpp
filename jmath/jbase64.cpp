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
#include "jbase64.h"

namespace jmath {

static int8_t jBase64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define jPad64	'='

static uint8_t jBase64_rank[256] = {
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0x00-0x0f	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0x10-0x1f	*/
	255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63, /*	0x20-0x2f	*/
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,255,255,255, /*	0x30-0x3f	*/
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /*	0x40-0x4f	*/
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255, /*	0x50-0x5f	*/
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /*	0x60-0x6f	*/
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255, /*	0x70-0x7f	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0x80-0x8f	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0x90-0x9f	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xa0-0xaf	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xb0-0xbf	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xc0-0xcf	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xd0-0xdf	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xe0-0xef	*/
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*	0xf0-0xff	*/
};

Base64::Base64()
{
}

Base64::~Base64()
{
}

char * Base64::Encode(uint8_t *src, int length, int *ret_length) {
	char* dst;
	int dstpos;
	char input[3];
	char output[4];
	// int ocnt = 0;
	int i;

	if (length == 0) 
		return NULL;	/* FIX: Or return ""? */

	/* Calculate required length of dst.  4 bytes of dst are needed for
	   every 3 bytes of src. */
	*ret_length = (((length + 2) / 3) * 4)+5;
	
	/* CHANGE::
	if (strict)
		*ret_length += (*ret_length / 72);	// Handle trailing \n 
	*/

	dst = new char[*ret_length];

	/* bulk encoding */
	dstpos = 0;
	while (length >= 3) 
	{
		/*
		   Convert 3 bytes of src to 4 bytes of output

		   output[0] = input[0] 7:2
		   output[1] = input[0] 1:0 input[1] 7:4
		   output[2] = input[1] 3:0 input[2] 7:6
		   output[3] = input[1] 5:0

*/
		input[0] = *src++;
		input[1] = *src++;
		input[2] = *src++;
		length -= 3;

		output[0] = (input[0] >> 2);
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		output[3] = (input[2] & 0x3f);

		// CHANGE:: g_assert ((dstpos + 4) < *ret_length);
		if ((dstpos + 4) >= *ret_length) {
			delete dst;
			return NULL;
		}

		/* Map output to the Base64 alphabet */
		dst[dstpos++] = jBase64[(uint32_t) output[0]];
		dst[dstpos++] = jBase64[(uint32_t) output[1]];
		dst[dstpos++] = jBase64[(uint32_t) output[2]];
		dst[dstpos++] = jBase64[(uint32_t) output[3]];

		/* Add a newline if strict and  */
		/* CHANGE::
		if (strict)
			if ((++ocnt % (72/4)) == 0) 
				dst[dstpos++] = '\n';
		*/
	}

	/* Now worry about padding with remaining 1 or 2 bytes */
	if (length != 0) 
	{
		input[0] = input[1] = input[2] = '\0';
		for (i = 0; i < length; i++) 
			input[i] = *src++;

		output[0] = (input[0] >> 2);
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);

		// CHANGE::: g_assert ((dstpos + 4) < *ret_length);
		if ((dstpos + 4) >= *ret_length) {
			delete dst;
			return NULL;
		}

		dst[dstpos++] = jBase64[(uint32_t) output[0]];
		dst[dstpos++] = jBase64[(uint32_t) output[1]];

		if (length == 1)
			dst[dstpos++] = jPad64;
		else
			dst[dstpos++] = jBase64[(uint32_t) output[2]];

		dst[dstpos++] = jPad64;
	}

	// CHANGE:: g_assert (dstpos <= *ret_length);
	if ((dstpos) > *ret_length) {
		delete dst;
		return NULL;
	}

	dst[dstpos] = '\0';

	*ret_length = dstpos + 1;

	return dst;
}

char * Base64::Decode(uint8_t *src, int length, int *ret_length) {
	char* dst;
	int   dstidx, state, ch = 0;
	char  res;
	uint8_t pos;

	if (length == 0) 
		length = strlen((const char *)src);
	state = 0;
	dstidx = 0;
	res = 0;

	dst = new char[length+1];
	*ret_length = length+1;

	while (length-- > 0) 
	{
		ch = *src++;
		if (jBase64_rank[ch]==255) /* Skip any non-base64 anywhere */
			continue;
		if (ch == jPad64) 
			break;

		pos = jBase64_rank[ch];

		switch (state) 
		{
			case 0:
				if (dst != NULL) 
				{
					dst[dstidx] = (pos << 2);
				}
				state = 1;
				break;
			case 1:
				if (dst != NULL) 
				{
					dst[dstidx] |= (pos >> 4);
					res = ((pos & 0x0f) << 4);
				}
				dstidx++;
				state = 2;
				break;
			case 2:
				if (dst != NULL) 
				{
					dst[dstidx] = res | (pos >> 2);
					res = (pos & 0x03) << 6;
				}
				dstidx++;
				state = 3;
				break;
			case 3:
				if (dst != NULL) 
				{
					dst[dstidx] = res | pos;
				}
				dstidx++;
				state = 0;
				break;
			default:
				break;
		}
	}
	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with erroneous trailing characters.
	 */
	if (ch == jPad64)           /* We got a pad char. */
	{
		switch (state) 
		{
			case 0:             /* Invalid = in first position */
			case 1:             /* Invalid = in second position */
				return NULL;
			case 2:             /* Valid, means one byte of info */
				/* Skip any number of spaces. */
				while (length-- > 0) 
				{
					ch = *src++;
					if (jBase64_rank[ch] != 255) break;
				}
				/* Make sure there is another trailing = sign. */
				if (ch != jPad64) 
				{
					free(dst);
					*ret_length = 0;
					return NULL;
				}
				/* FALLTHROUGH */
			case 3:             /* Valid, means two bytes of info */
				/*
				 * We know this char is an =.  Is there anything but
				 * whitespace after it?
				 */
				while (length-- > 0) 
				{
					ch = *src++;
					if (jBase64_rank[ch] != 255) 
					{
						free(dst);
						*ret_length = 0;
						return NULL;
					}
				}
				/*
				 * Now make sure for cases 2 and 3 that the "extra"
				 * bits that slopped past the last full byte were
				 * zeros.  If we don't check them, they become a
				 * subliminal channel.
				 */
				if (dst != NULL && res != 0) 
				{
					free(dst);
					*ret_length = 0;
					return NULL;
				}
			default:
				break;
		}
	} else 
	{
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state != 0) 
		{
			free(dst);
			*ret_length = 0;
			return NULL;
		}
	}
	dst[dstidx]=0;
	*ret_length = dstidx;
	return dst;
}

}















