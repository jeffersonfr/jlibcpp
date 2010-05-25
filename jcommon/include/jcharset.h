/***************************************************************************
 *   Copyright (C) 2005 by <still unknown> <modified version>              *
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
#ifndef J_CHARSET_H
#define J_CHARSET_H

#include "jobject.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef uint32_t	UTF32;		// at least 32 bits 
typedef uint16_t	UTF16;		// at least 16 bits 
typedef uint8_t		UTF8;		// typically 8 bits 
typedef uint32_t	Char; 		// 0 or 1 

// Some fundamental constants 
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

enum jcharset_result_t {
	CS_OK, 					// conversion successful 
	CS_SOURCE_EXHAUSTED,	// partial character in source, but hit end 
	CS_TARGET_EXHAUSTED,	// insuff. room in target for conversion 
	CS_SOURCE_ILLEGAL,		// source sequence is illegal/malformed 
	CS_SOURCE_CORRUPT		// source was corrupted 
};

enum jcharset_flags_t {
	strictConversion = 0,
	lenientConversion
};

enum {
	Low6Bits = 0x3F,	// 00111111 
	High2Bits = 0xC0,	// 11000000 
	ByteMask = 0x00BF,	// 10111111 
	ContinueBits = 0x80	// 10xxxxxx 
};

namespace jcommon {

/**
 * \brief UTF-8 is a way of reading and writing Unicode 32-bit characters
 *  to ordinary 8-bit communications streams.
 *
 *  The UTF-8 algorithm stores characters into variable-sized
 *  chunks. Characters in the range 0x00 to 0x7F fit into one
 *  byte, since these will be quite common (ASCII values).
 *  Characters with higher values fit into two, three, four,
 *  five, or six bytes, depending on the number of significant
 *  bits, according to the following pattern:
 *
 *  Bits  Pattern
 *  ----  -------
 *    7   0xxxxxxx
 *   11   110xxxxx 10xxxxxx
 *   16   1110xxxx 10xxxxxx 10xxxxxx
 *   21   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *   26   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *   32   111111xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 *  As can be seen from the table, at most 32 bits can be stored
 *  using this algorithm (the x's mark where the actual bits go,
 *  the numbers signify the padding bits). The padding "10" at
 *  the start of a byte uniquely identifies a continuation byte,
 *  which is never used as the start of a UTF-8 character sequence,
 *  so if a stream is broken for some reason, the algorithm can
 *  skip those bytes to find the next start of a character.
 *
 *  ASCII is a 7-bit encoding for the English language alphabet
 *  and various digits and symbols. Its values range from 0x00 to 0x7F.
 *
 *  A superset of ASCII is ISO-Latin-1 (code page 8859-1). This is
 *  an 8-bit encoding for Western European languages, with values
 *  in the range 0x00 to 0xFF. The lower half of this range is
 *  the same as ASCII, while the upper half includes many accented
 *  characters.
 *
 *  Unicode is a superset of ISO-Latin-1, which mostly fits into
 *  16-bits, but which is actually a 32-bit encoding for most
 *  language symbols on Earth, including Eastern European, African,
 *  Asian, and many other languages. It allows a single document
 *  to contain mixtures of all languages.
 *
 *  This file contains functions for reading and writing Unicode
 *  and ISO-Latin-1 streams, to and from an array of 32-bit
 *  Unicode values in memory. Each 32-bit value is called a Char.
 */
class Charset : public virtual jcommon::Object{

	private:

		
	public:
		/**
		 * \brief
		 * 
		 */
		Charset();

		/**
		 * \brief
		 *
		 */
		virtual ~Charset();

		/**
		 * \brief
		 *
		 */
		jcharset_result_t UnicodeToUTF8(const Char **src_start, const Char *src_end, char **dst_start, const char *dst_end);

		/**
		 * \brief
		 *
		 */
		jcharset_result_t UTF8ToUnicode(const char **src_start, const char *src_end, Char **dst_start, const Char *dst_end);

		/*
		 *  \brief Convert a single Unicode character into a UTF-8 byte buffer.
		 *  The buffer should be at least 7 bytes wide.
		 */
		int UnicodeToUTF8(Char ch, char *utf8);

		/*
		 *  \brief Convert a single UTF-8 character into a Unicode value.
		 */
		Char UTF8ToUnicode(const char *utf8);

		/*
		 * \brief Return the number of Unicode characters within a UTF-8 string.
		 * For ASCII strings this will return the same number as strlen.
		 */
		int UTF8Length(const char *utf8);

		/*
		 * \brief Read a Unicode value from a UTF-8 file. The file must be open in 
		 * binary mode to read. Errors are reported in the return value, as a charset_result_t.
		 */
		int ReadUTF8(FILE *f, Char * dst);

		/*
		 * \brief Read a file into a UTF-8 char array, up to and including
		 *  the 'stop' character (or an EOF will end input).
		 *  This function returns the alloc'd UTF-8 encoded string.
		 *  The number of bytes in the returned string is placed in *nbytes.
		 *  The number of characters in the returned string is placed in *nchars.
		 *  If EOF is encountered immediately, the function returns NULL.
		 *  If the 'stop' character is EOF, this function reads the
		 *  entire file.
		 */
		char * ReadLatin1File(FILE *f, int *nbytes, int *nchars, int stop);

		/**
		 * \brief
		 *
		 */
		char * ReadUTF8File(FILE *f, int *nbytes, int *nchars, int stop);

		/*
		 * \brief Read an entire file into a memory char array. Return NULL if the file is empty.
		 *
		 */
		char * ReadLatin1Buffer(FILE *f, int *nbytes, int *nchars);
		
		/**
		 * \brief
		 *
		 */
		char * ReadUTF8Buffer(FILE *f, int *nbytes, int *nchars);

		/*
		 * \brief A function for reading one line of input from a file.
		 *  This function returns the alloc'd string, and any
		 *  terminating newline character is included in the line.
		 *  The length of the returned string is placed into *length.
		 *  If EOF is encountered immediately, the function returns NULL.
		 *  If EOF is encountered before a newline character, the string
		 *  is returned without any terminating newline.
		 *  Otherwise, a newline character will be the last character
		 *  in the char array.
		 */
		char * ReadLatin1Line(FILE *f, int *nbytes, int *nchars);
		
		/**
		 * \brief
		 *
		 */
		char * ReadUTF8Line(FILE *f, int *nbytes, int *nchars);

		/*
		 * \brief Write a UTF-8 char array to a file as ISO Latin 1 (non ISO-Latin-1 characters 
		 * will be distorted by this process). Assume the UTF-8 char array is correct.
		 */
		int WriteLatin1(FILE *f, const char *utf8, int nbytes);

		/*
		 * \brief Write a UTF-8 char array to a file as UTF-8. Assume the UTF-8 char array is correct.
		 *
		 */
		int WriteUTF8(FILE *f, const char *utf8, int nbytes);

		/*
		 *  Convert a UTF-8 char array to an ISO Latin 1 char array (non ISO Latin 1 characters 
		 *  will be distorted by this process). Assume the UTF-8 char array is correct.
		 *  This function creates a new string containing the ISO Latin 1 data. It returns NULL 
		 *  if it runs out of memory.
		 */
		char * UTF8ToLatin1(const char *utf8, int *bytes);

		/*
		 * \brief Convert a (possibly ISO Latin 1) char array to a UTF-8 char array, as best we can. 
		 * If it is already correctly UTF 8 encoded, return the input string unchanged.  This function 
		 * may create a new string containing the UTF-8 data. It returns NULL if it runs out of memory.
		 */
		char * CorrectUTF8(const char *s, int *bytes);

		/*
		 * \brief Return non-zero (true) if the given UTF-8 char array contains
		 * only ASCII characters, otherwise return zero. 
		 */
		int IsASCII(const char *utf8, int nbytes);

		/*
		 * \brief Return non-zero (true) if the given UTF-8 char array contains
		 *  only ASCII and ISO Latin-1 characters, otherwise return zero. 
		 */
		int IsLatin1(const char *utf8, int nbytes);

		/**
		 * \brief
		 *
		 */
		jcharset_result_t ConvertUTF32ToUTF16(const UTF32** sourceStart, const UTF32* sourceEnd, UTF16** targetStart, UTF16* targetEnd, jcharset_flags_t flags);
		
		/**
		 * \brief
		 *
		 */
		jcharset_result_t ConvertUTF16ToUTF32(const UTF16** sourceStart, const UTF16* sourceEnd, UTF32** targetStart, UTF32* targetEnd, jcharset_flags_t flags);

		/* 
		 * \brief The interface converts a whole buffer to avoid function-call overhead.
		 * Constants have been gathered. Loops & conditionals have been removed as
		 * much as possible for efficiency, in favor of drop-through switches.
		 * (See "Note A" at the bottom of the file for equivalent code.)
		 * If your compiler supports it, the "isLegalUTF8" call can be turned
		 * into an inline function.
		 */

		jcharset_result_t ConvertUTF16ToUTF8(const UTF16 **sourceStart, const UTF16 *sourceEnd, UTF8 **targetStart, UTF8 *targetEnd, jcharset_flags_t flags);

		/*
		 * \brief Utility routine to tell whether a sequence of bytes is legal UTF-8.
		 * This must be called with the length pre-determined by the first byte.
		 * If not calling this from ConvertUTF8to*, then the length can be set by:
		 *  length = trailingBytesForUTF8[*source]+1;
		 * and the sequence is illegal right away if there aren't that many bytes available.
		 * If presented with a length > 4, this returns false. The Unicode
		 * definition of UTF-8 goes up to 4-byte sequences.
		 */
		bool IsLegalUTF8(const UTF8 *source, int length);

		/*
		 * \brief Exported function to return whether a UTF-8 sequence is legal or not.
		 * This is not used here; it's just exported.
		 *
		 */
		bool IsLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd);
		
		/**
		 * \brief
		 *
		 */
		jcharset_result_t ConvertUTF8ToUTF16(const UTF8** sourceStart, const UTF8* sourceEnd, UTF16** targetStart, UTF16* targetEnd, jcharset_flags_t flags);

		/**
		 * \brief
		 *
		 */
		jcharset_result_t ConvertUTF32ToUTF8(const UTF32** sourceStart, const UTF32* sourceEnd, UTF8** targetStart, UTF8* targetEnd, jcharset_flags_t flags);

		/**
		 * \brief
		 *
		 */
		jcharset_result_t ConvertUTF8ToUTF32(const UTF8 **sourceStart, const UTF8 *sourceEnd, UTF32 **targetStart, UTF32 *targetEnd, jcharset_flags_t flags);

};

}

#endif

























