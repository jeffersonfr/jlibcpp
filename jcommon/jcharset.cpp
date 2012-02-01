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
#include "Stdafx.h"
#include "jcharset.h"

static const uint64_t ReplacementChar = 0x0000FFFDUL;
static const uint64_t MaximumChar     = 0x7FFFFFFFUL;

static const uint8_t UTF8ExtraBytes[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

static const uint8_t FirstByteBits[7] = {
	0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const uint64_t FirstByteMask[6] = {
	0xFF, 0x1F, 0x0F, 0x07, 0x03, 0x03
};

/*
 * \brief Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was allowed in earlier algorithms.
 */
static const char trailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * \brief Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes in a UTF-8 sequence.
 */
static const UTF32 offsetsFromUTF8[6] = { 
	0x00000000UL, 0x00003080UL, 0x000E2080UL, 
	0x03C82080UL, 0xFA082080UL, 0x82082080UL 
};

/*
 * \brief Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const int halfShift  = 10; // used for shifting by 10 bits 

static const UTF32 halfBase = 0x0010000UL;
static const UTF32 halfMask = 0x3FFUL;

#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF

namespace jcommon {

Charset::Charset():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Charset");
}

Charset::~Charset()
{
}

jcharset_result_t Charset::UnicodeToUTF8(const Char **src_start, const Char *src_end, char **dst_start, const char *dst_end)
{
	jcharset_result_t result = JCR_OK;
	register const Char * src = *src_start;
	register char * dst = *dst_start;

	while (src < src_end) {
		register uint64_t ch;
		register uint32_t bytes_to_write,
						 extra_bytes;

		ch = *src++;

		if (ch < 0x80) {
			bytes_to_write = 1;
		} else if (ch < 0x800) {
			bytes_to_write = 2;
		} else if (ch < 0x10000) {
			bytes_to_write = 3;
		} else if (ch < 0x200000) {
			bytes_to_write = 4;
		} else if (ch < 0x4000000) {
			bytes_to_write = 5;
		} else if (ch <= MaximumChar) {
			bytes_to_write = 6;
		} else {
			bytes_to_write = 2;
			ch = ReplacementChar;
		}

		dst += bytes_to_write;
		if (dst > dst_end) {
			dst -= bytes_to_write;
			result = JCR_TARGET_EXHAUSTED;
			break;
		}
		extra_bytes = bytes_to_write;
		while (--extra_bytes) {
			*--dst = (char) ((ch | ContinueBits) & ByteMask);
			ch >>= 6;
		}
		*--dst = (char) (ch | FirstByteBits[bytes_to_write]);

		dst += bytes_to_write;
	}
	*src_start = src;
	*dst_start = dst;
	return result;
}

jcharset_result_t Charset::UTF8ToUnicode(const char **src_start, const char *src_end, Char **dst_start, const Char *dst_end)
{
	jcharset_result_t result = JCR_OK;
	register const char * src = *src_start;
	register Char * dst = *dst_start;

	while (src < src_end) {
		register Char ch;
		register unsigned short extra_bytes;

		extra_bytes = UTF8ExtraBytes[(uint8_t)*src];
		/*
		if (src + extra_bytes > src_end) {
			result |= SourceExhausted;
			break;
		}
		*/
		if (dst >= dst_end) {
			result = (jcharset_result_t)(result | JCR_TARGET_EXHAUSTED);
			break;
		}

		ch = (uint8_t)*src++;
		if (extra_bytes) {
			if (src >= src_end) {
				result = (jcharset_result_t)(result | JCR_SOURCE_CORRUPT);
				result = (jcharset_result_t)(result | JCR_SOURCE_EXHAUSTED);
			}
			else if ((*src & High2Bits) == ContinueBits) {
				ch &= FirstByteMask[extra_bytes];
				do {
					ch <<= 6;
					ch |= ((*src++) & Low6Bits);
					if (--extra_bytes == 0)
						break;
					if (src >= src_end) {
						result = (jcharset_result_t)(result | JCR_SOURCE_CORRUPT);
						result = (jcharset_result_t)(result | JCR_SOURCE_EXHAUSTED);
						break;
					}
					if ((*src & High2Bits) != ContinueBits) {
						result = (jcharset_result_t)(result | JCR_SOURCE_CORRUPT);
						break;
					}
				} while (1);
			} else {
				result = (jcharset_result_t)(result | JCR_SOURCE_CORRUPT);
			}
		}

		if (ch <= MaximumChar) {
			*dst++ = ch;
		} else {
			*dst++ = ReplacementChar;
		}
	}
	*src_start = src;
	*dst_start = dst;

	return result;
}

int Charset::UnicodeToUTF8(Char ch, char *utf8)
{
	char *bp;
	const Char *cp;

	cp = &ch;
	bp = utf8;
	memset(utf8, 0, 7);

	return UnicodeToUTF8(&cp, cp+1, &bp, bp+7);
}

Char Charset::UTF8ToUnicode(const char *utf8)
{
	const char *bp;
	Char ch;
	Char *cp;

	cp = &ch;
	bp = utf8;
	
	UTF8ToUnicode(&bp, bp+7, &cp, cp+1);
	
	return ch;
}

int Charset::UTF8Length(const char *utf8)
{
	int length = 0;

	while (*utf8 != '\0') {
		length++;
		if ((*utf8 & High2Bits) == High2Bits) {
			utf8++;
			while ((*utf8 & High2Bits) == ContinueBits)
				utf8++;
		}
		else
			utf8++;
	}
	return length;
}

int Charset::ReadUTF8(FILE *f, Char * dst)
{
	jcharset_result_t result = JCR_OK;
	Char ch;
	unsigned short extra_bytes;
	int c;

	c = getc(f);
	if (c == EOF) {
		*dst = '\0';
		return 4; // SourceExhausted;
	}

	extra_bytes = UTF8ExtraBytes[c];

	ch = c;
	if (extra_bytes) {
		c = getc(f);
		if (c == EOF) {
			*dst = ch;
			return 4; // SourceExhausted;
		}
		if ((c & High2Bits) == ContinueBits) {
			ch &= FirstByteMask[extra_bytes];
			do {
				ch <<= 6;
				ch |= (c & Low6Bits);
				if (--extra_bytes == 0)
					break;
				c = getc(f);
				if (c == EOF) {
					result = JCR_SOURCE_EXHAUSTED;
					break;
				}
				if ((c & High2Bits) != ContinueBits) {
					ungetc(c, f);
					result = JCR_SOURCE_CORRUPT;
					break;
				}
			} while (1);
		} else {
			result = JCR_SOURCE_CORRUPT;
		}
	}

	if (ch <= MaximumChar) {
		*dst = ch;
	} else {
		*dst = ReplacementChar;
	}
	return result;
}

char * Charset::ReadLatin1File(FILE *f, int *nbytes, int *nchars, int stop)
{
	int pos, 
		nch, 
		max;
	char *line;
	char *c;
	int ch;

	if (feof(f)) {
		*nbytes = 0;
		*nchars = 0;
		return NULL;
	}

	pos = 0;
	nch = 0;
	max = 112; /* big enough for many input lines; not a power of 2 */
	line = (char *)malloc(max);
	c = & line[pos];

	while ((ch = getc(f)) != EOF) {
		if (ch & 0x80) {
			/* character will occupy two bytes */
			if (pos+2 >= max) {
				/* realloc buffer twice the size */
				max += max;
				line = (char *)realloc(line, max);
				c = & line[pos];
			}
			*c = (0xC0 | ((ch >> 6) & 0x03)); /* 110---xx */
			c++;
			pos++;

			*c = (0x80 | (ch & 0x3F)); /* 10xxxxxx */
			c++;
			pos++;

			nch++;
		}
		else {
			/* character will only occupy one byte */
			*c = (ch & 0x7F);
			c++;
			pos++;
			if (pos == max) {
				/* realloc buffer twice the size */
				max += max;
				line = (char *)realloc(line, max);
				c = & line[pos];
			}
			nch++;
		}

		if (ch == stop) {
			break;
		}
	}

	/* shrink array to smallest required space */
	line = (char *)realloc(line, pos+1);
	line[pos] = '\0';

	*nbytes = pos;
	*nchars = nch;

	return line;
}

char * Charset::ReadUTF8File(FILE *f, int *nbytes, int *nchars, int stop)
{
	int pos,
			nch, 
			max;
	char *line;
	char *c;
	int ch;

	if (feof(f)) {
		*nbytes = 0;
		*nchars = 0;
		return NULL;
	}

	pos = 0;
	nch = 0;
	max = 112; /* big enough for many input lines; not a power of 2 */
	line = (char *)malloc(max);
	c = & line[pos];

	while ((ch = getc(f)) != EOF) {
		*c = ch;
		c++;
		pos++;

		if (pos == max) {
			/* realloc buffer twice the size */
			max += max;
			line = (char *)realloc(line, max);
			c = & line[pos];
		}
		
		// i.e. not 10xxxxxx
		if ((ch & 0xC0) != 0xC0) {
			nch++;
		}
		
		if (ch == (int)stop) {
			break;
		}
	}

	/* shrink array to smallest required space */
	line = (char *)realloc(line, pos+1);
	line[pos] = '\0';

	*nbytes = pos;
	*nchars = nch;
	return line;
}

char *Charset::ReadLatin1Buffer(FILE *f, int *nbytes, int *nchars)
{
	return ReadLatin1File(f, nbytes, nchars, EOF);
}

char *Charset::ReadUTF8Buffer(FILE *f, int *nbytes, int *nchars)
{
	return ReadUTF8File(f, nbytes, nchars, EOF);
}

char *Charset::ReadLatin1Line(FILE *f, int *nbytes, int *nchars)
{
	return ReadLatin1File(f, nbytes, nchars, '\n');
}

char *Charset::ReadUTF8Line(FILE *f, int *nbytes, int *nchars)
{
	return ReadUTF8File(f, nbytes, nchars, '\n');
}

int Charset::WriteLatin1(FILE *f, const char *utf8, int nbytes)
{
	Char buf[1];
	Char *bp;
	char ch;
	const char *sp;
	size_t total = 0;

	sp = utf8;
	while (nbytes > 0) {
		/* write one UTF-8 sequence into a Char buffer */
		bp = buf;
		UTF8ToUnicode(&sp, sp+7, &bp, bp+1);
		/* determine what happened */
		nbytes -= (int64_t) (sp - utf8);
		utf8 = sp;
		/* force Unicode Char into a Latin-1 char */
		ch = (char) (buf[0] & 0x00FF);
		putc(ch, f);
		total++;
	}

	fflush(f);
	
	return total;
}

int Charset::WriteUTF8(FILE *f, const char *utf8, int nbytes)
{
	int n, 
		total = 0;

	while (nbytes > 256) {
		n = (int64_t) fwrite(utf8, 1, 256, f);
		total += n;
		nbytes -= n;
	}

	total += fwrite(utf8, 1, nbytes, f);
	
	fflush(f);
	
	return total;
}

char * Charset::UTF8ToLatin1(const char *utf8, int *bytes)
{
	Char buf[1];
	Char *bp;
	const char *sp;
	int total = 0,
			nbytes = *bytes;
	char *dp;
	char *dest;

	dest = (char *)malloc(nbytes + 1);
	if (dest == NULL) {
		*bytes = 0;
		return NULL;
	}
	dp = dest;

	sp = utf8;
	while (nbytes > 0) {
		/* write one UTF-8 sequence into a Char buffer */
		bp = buf;
		UTF8ToUnicode(&sp, sp+7, &bp, bp+1);
		/* determine what happened */
		nbytes -= (int64_t) (sp - utf8);
		utf8 = sp;
		/* force Unicode Char into a Latin-1 char */
		*dp++ = (char) (buf[0] & 0x00FF);
		total++;
	}
	*dp = '\0';
	*bytes = total;
	return dest;
}

char * Charset::CorrectUTF8(const char *s, int *bytes)
{
	Char buf[1];
	Char *bp;
	const Char *cbp;
	const char *sp;
	int total,
		nbytes = *bytes;
	char *dp;
	char *dest;
	char tmp[8];
	char *tp;
	const char *original;
	int i;
	int diff = 0;

	total = nbytes;
	dest = (char *)malloc(total + 1);
	if (dest == NULL) {
		*bytes = 0;
		return NULL;
	}
	dp = dest;

	original = s;
	sp = s;
	while (nbytes > 0) {
		/* write one UTF-8 sequence into a Char buffer */
		bp = buf;
		UTF8ToUnicode(&sp, sp+7, &bp, bp+1);

		/* determine what happened */
		nbytes -= (size_t) (sp - s);

		/* check for any difference, copy string also */
		cbp = buf;
		tp = tmp;
		UnicodeToUTF8(&cbp, cbp+1, &tp, tp+7);
		i = (int) (dp-dest);
		if (total-i < tp-tmp) {
			total = i + (size_t) (tp-tmp);
			dest = (char *)realloc(dest, total+1);
			dp = dest + i;
		}
		for (i=0; i < (tp-tmp); i++) {
			if (s[i] != tmp[i])
				diff = 1;
			*dp++ = tmp[i];
		}

		/* move on to next char */
		s = sp;
	}
	*dp = '\0';
	*bytes = total;
	if (diff == 0) {
		free(dest);
		return (char *) original; /* cast away const */
	}
	return dest;
}

int Charset::IsASCII(const char *utf8, int nbytes)
{
	for (int i=0; i < nbytes; i++) {
		if (*utf8 & 0x80)
			return 0;
		utf8++;
	}

	return 1;
}

int Charset::IsLatin1(const char *utf8, int nbytes)
{
	for (int i=0; i < (int)nbytes; i++) {
		if (*utf8 & 0x80) {
			// not 110-000xx
			if ((*utf8 & 0xFC) != 0xC0) {
				return 0;
			}

			utf8++;
			i++;
	
			if (i >= nbytes) {
				return 0;
			}

			// not 10-xxxxxx
			if ((*utf8 & 0xC0) != 0x80) {
				return 0;
			}
		}
		utf8++;
	}

	return 1;
}

jcharset_result_t Charset::ConvertUTF32ToUTF16(const UTF32** sourceStart, const UTF32* sourceEnd, UTF16** targetStart, UTF16* targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF32* source = *sourceStart;
	UTF16* target = *targetStart;
	
	while (source < sourceEnd) {
		UTF32 ch;
		
		if (target >= targetEnd) {
			result = JCR_TARGET_EXHAUSTED; 
			break;
		}
		
		ch = *source++;
		
		if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
			/* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				if (flags == JCF_STRICT_CONVERSION) {
					--source; /* return to the illegal value itself */
					result = JCR_SOURCE_ILLEGAL;
					break;
				} else {
					*target++ = UNI_REPLACEMENT_CHAR;
				}
			} else {
				*target++ = (UTF16)ch; /* normal case */
			}
		} else if (ch > UNI_MAX_LEGAL_UTF32) {
			if (flags == JCF_STRICT_CONVERSION) {
				result = JCR_SOURCE_ILLEGAL;
			} else {
				*target++ = UNI_REPLACEMENT_CHAR;
			}
		} else {
			/* target is a character in range 0xFFFF - 0x10FFFF. */
			if (target + 1 >= targetEnd) {
				--source; /* Back up source pointer! */
				result = JCR_SOURCE_EXHAUSTED;
			   	break;
			}

			ch -= halfBase;
			*target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
		}
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}

jcharset_result_t Charset::ConvertUTF16ToUTF32(const UTF16** sourceStart, const UTF16* sourceEnd, UTF32** targetStart, UTF32* targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF16* source = *sourceStart;
	UTF32* target = *targetStart;
	UTF32 ch, ch2;
	
	while (source < sourceEnd) {
		const UTF16* oldSource = source; /*  In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd) {
				ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
					+ (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				} else if (flags == JCF_STRICT_CONVERSION) { /* it's an unpaired high surrogate */
					--source; /* return to the illegal value itself */
					result = JCR_SOURCE_ILLEGAL;
					break;
				}
			} else { /* We don't have the 16 bits following the high surrogate. */
				--source; /* return to the high surrogate */
				result = JCR_SOURCE_EXHAUSTED;
				break;
			}
		} else if (flags == JCF_STRICT_CONVERSION) {
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
				--source; /* return to the illegal value itself */
				result = JCR_SOURCE_ILLEGAL;
				break;
			}
		}
		if (target >= targetEnd) {
			source = oldSource; /* Back up source pointer! */
			result = JCR_TARGET_EXHAUSTED; 
			break;
		}
		*target++ = ch;
	}
	*sourceStart = source;
	*targetStart = target;
#ifdef CVTUTF_DEBUG
	if (result == JCR_SOURCE_ILLEGAL) {
		fprintf(stderr, "ConvertUTF16toUTF32 illegal seq 0x%04x,%04x\n", ch, ch2);
		fflush(stderr);
	}
#endif

	return result;
}

jcharset_result_t Charset::ConvertUTF16ToUTF8(const UTF16 **sourceStart, const UTF16 *sourceEnd, UTF8 **targetStart, UTF8 *targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF16* source = *sourceStart;
	UTF8* target = *targetStart;
	
	while (source < sourceEnd) {
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80; 
		const UTF16* oldSource = source; /* In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd) {
				UTF32 ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
					+ (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				} else if (flags == JCF_STRICT_CONVERSION) { 
					// it's an unpaired high surrogate 
					--source; // return to the illegal value itself 
					result = JCR_SOURCE_ILLEGAL;
					break;
				}
			} else { 
				// We don't have the 16 bits following the high surrogate. 
				--source; // return to the high surrogate 
				result = JCR_SOURCE_EXHAUSTED;
				break;
			}
		} else if (flags == JCF_STRICT_CONVERSION) {
			// UTF-16 surrogate values are illegal in UTF-32 
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
				--source; // return to the illegal value itself 
				result = JCR_SOURCE_ILLEGAL;
				break;
			}
		}

		// Figure out how many bytes the result will require 
		if (ch < (UTF32)0x80) {	     
			bytesToWrite = 1;
		} else if (ch < (UTF32)0x800) {     
			bytesToWrite = 2;
		} else if (ch < (UTF32)0x10000) {   
			bytesToWrite = 3;
		} else if (ch < (UTF32)0x110000) {  
			bytesToWrite = 4;
		} else {			    
			bytesToWrite = 3;
			ch = UNI_REPLACEMENT_CHAR;
		}
		
		target += bytesToWrite;
		if (target > targetEnd) {
			source = oldSource; // Back up source pointer! 
			target -= bytesToWrite; 
			result = JCR_TARGET_EXHAUSTED; 
			break;
		}

		switch (bytesToWrite) { // note: everything falls through. 
			case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 1: *--target =  (UTF8)(ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}

bool Charset::IsLegalUTF8(const UTF8 *source, int length) 
{
	const UTF8 *srcptr = source+length;
	UTF8 a;
	
	switch (length) {
		default: return false;
			/* Everything else falls through when "true"... */
		case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 2: if ((a = (*--srcptr)) > 0xBF) return false;
			
			switch (*source) {
				/* no fall-through in this inner switch */
				case 0xE0: if (a < 0xA0) return false; break;
				case 0xED: if (a > 0x9F) return false; break;
				case 0xF0: if (a < 0x90) return false; break;
				case 0xF4: if (a > 0x8F) return false; break;
				default:   if (a < 0x80) return false;
			}
				
			case 1: if (*source >= 0x80 && *source < 0xC2) return false;
	}
	if (*source > 0xF4) return false;
	return true;
}

bool Charset::IsLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd) 
{
	int length = trailingBytesForUTF8[*source]+1;
	
	if (source+length > sourceEnd) {
		return false;
	}
	
	return IsLegalUTF8(source, length);
}

jcharset_result_t Charset::ConvertUTF8ToUTF16(const UTF8** sourceStart, const UTF8* sourceEnd, UTF16** targetStart, UTF16* targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF8* source = *sourceStart;
	UTF16* target = *targetStart;
	
	while (source < sourceEnd) {
		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
		if (source + extraBytesToRead >= sourceEnd) {
			result = JCR_SOURCE_EXHAUSTED;
			break;
		}
	
		// Do this check whether lenient or strict 
		if (! IsLegalUTF8(source, extraBytesToRead+1)) {
			result = JCR_SOURCE_ILLEGAL;
			break;
		}
		
		// The cases all fall through. See "Note A" below.
		switch (extraBytesToRead) {
			case 5: ch += *source++; ch <<= 6; // remember, illegal UTF-8 
			case 4: ch += *source++; ch <<= 6; // remember, illegal UTF-8 
			case 3: ch += *source++; ch <<= 6;
			case 2: ch += *source++; ch <<= 6;
			case 1: ch += *source++; ch <<= 6;
			case 0: ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];
		
		if (target >= targetEnd) {
			source -= (extraBytesToRead+1); // Back up source pointer! 
			result = JCR_TARGET_EXHAUSTED; 
			break;
		}
		if (ch <= UNI_MAX_BMP) { 
			// Target is a character <= 0xFFFF 
			// UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				if (flags == JCF_STRICT_CONVERSION) {
					source -= (extraBytesToRead+1); /* return to the illegal value itself */
					result = JCR_SOURCE_ILLEGAL;
					break;
				} else {
					*target++ = UNI_REPLACEMENT_CHAR;
				}
			} else {
				*target++ = (UTF16)ch; /* normal case */
			}
		} else if (ch > UNI_MAX_UTF16) {
			if (flags == JCF_STRICT_CONVERSION) {
				result = JCR_SOURCE_ILLEGAL;
				source -= (extraBytesToRead+1); /* return to the start */
				break; /* Bail out; shouldn't continue */
			} else {
				*target++ = UNI_REPLACEMENT_CHAR;
			}
		} else {
			/* target is a character in range 0xFFFF - 0x10FFFF. */
			if (target + 1 >= targetEnd) {
				source -= (extraBytesToRead+1); /* Back up source pointer! */
				result = JCR_TARGET_EXHAUSTED; break;
			}
			ch -= halfBase;
			*target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
		}
	}
	
	*sourceStart = source;
	*targetStart = target;

	return result;
}

jcharset_result_t Charset::ConvertUTF32ToUTF8(const UTF32** sourceStart, const UTF32* sourceEnd, UTF8** targetStart, UTF8* targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF32* source = *sourceStart;
	UTF8* target = *targetStart;
	
	while (source < sourceEnd) {
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80; 
		ch = *source++;
		if (flags == JCF_STRICT_CONVERSION ) {
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				--source; /* return to the illegal value itself */
				result = JCR_SOURCE_ILLEGAL;
				break;
			}
		}
		
		// Figure out how many bytes the result will require. Turn any
		// illegally large UTF32 things (> Plane 17) into replacement chars.
		if (ch < (UTF32)0x80) {	     bytesToWrite = 1;
		} else if (ch < (UTF32)0x800) {     bytesToWrite = 2;
		} else if (ch < (UTF32)0x10000) {   bytesToWrite = 3;
		} else if (ch <= UNI_MAX_LEGAL_UTF32) {  bytesToWrite = 4;
		} else {			    bytesToWrite = 3;
			ch = UNI_REPLACEMENT_CHAR;
			result = JCR_SOURCE_ILLEGAL;
		}
		
		target += bytesToWrite;
		if (target > targetEnd) {
			--source; // Back up source pointer! 
			target -= bytesToWrite; 
			result = JCR_TARGET_EXHAUSTED; 
			break;
		}
		switch (bytesToWrite) { 
			// note: everything falls through. 
			case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
			case 1: *--target = (UTF8) (ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}

jcharset_result_t Charset::ConvertUTF8ToUTF32(const UTF8** sourceStart, const UTF8* sourceEnd, UTF32** targetStart, UTF32* targetEnd, jcharset_flags_t flags) 
{
	jcharset_result_t result = JCR_OK;
	const UTF8* source = *sourceStart;
	UTF32* target = *targetStart;
	
	while (source < sourceEnd) {
		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
		if (source + extraBytesToRead >= sourceEnd) {
			result = JCR_SOURCE_EXHAUSTED; break;
		}
		// Do this check whether lenient or strict 
		if (! IsLegalUTF8(source, extraBytesToRead+1)) {
			result = JCR_SOURCE_ILLEGAL;
			break;
		}
		// The cases all fall through. See "Note A" below.
		switch (extraBytesToRead) {
			case 5: ch += *source++; ch <<= 6;
			case 4: ch += *source++; ch <<= 6;
			case 3: ch += *source++; ch <<= 6;
			case 2: ch += *source++; ch <<= 6;
			case 1: ch += *source++; ch <<= 6;
			case 0: ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];
		
		if (target >= targetEnd) {
			source -= (extraBytesToRead+1); /* Back up the source pointer! */
			result = JCR_TARGET_EXHAUSTED; break;
		}
		if (ch <= UNI_MAX_LEGAL_UTF32) {
			// UTF-16 surrogate values are illegal in UTF-32, and anything over Plane 17 (> 0x10FFFF) is illegal.
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				if (flags == JCF_STRICT_CONVERSION) {
					source -= (extraBytesToRead+1); /* return to the illegal value itself */
					result = JCR_SOURCE_ILLEGAL;
					break;
				} else {
					*target++ = UNI_REPLACEMENT_CHAR;
				}
			} else {
				*target++ = ch;
			}
		} else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
			result = JCR_SOURCE_ILLEGAL;
			*target++ = UNI_REPLACEMENT_CHAR;
		}
	}

	*sourceStart = source;
	*targetStart = target;

	return result;
}

}

