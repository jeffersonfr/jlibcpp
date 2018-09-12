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
/* GdkPixbuf library - XBM image loader
 *
 * Copyright (C) 1999 Mark Crichton
 * Copyright (C) 1999 The Free Software Foundation
 * Copyright (C) 2001 Eazel, Inc.
 *
 * Authors: Mark Crichton <crichton@gimp.org>
 *          Federico Mena-Quintero <federico@gimp.org>
 *          Jonathan Blandford <jrb@redhat.com>
 *	    John Harper <jsh@eazel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include "generic/include/genericprovider_xbm.h"
#include "jio/jfileinputstream.h"
#include "jio/jmemoryinputstream.h"
#include "jio/jbufferedreader.h"

#include <string.h>

namespace jgui {

#define MAX_SIZE 255

/* shared data for the image read/parse logic */
static short hex_table[256];		/* conversion value */
static bool initialized = false;	/* easier to fill in at run time */


/* Table index for the hex values. Initialized once, first time.
 * Used for translation value or delimiter significance lookup.
 */
static void
init_hex_table (void)
{
	/*
	 * We build the table at run time for several reasons:
	 *
	 * 1. portable to non-ASCII machines.
	 * 2. still reentrant since we set the init flag after setting table.
	 * 3. easier to extend.
	 * 4. less prone to bugs.
	 */
	hex_table['0'] = 0;
	hex_table['1'] = 1;
	hex_table['2'] = 2;
	hex_table['3'] = 3;
	hex_table['4'] = 4;
	hex_table['5'] = 5;
	hex_table['6'] = 6;
	hex_table['7'] = 7;
	hex_table['8'] = 8;
	hex_table['9'] = 9;
	hex_table['A'] = 10;
	hex_table['B'] = 11;
	hex_table['C'] = 12;
	hex_table['D'] = 13;
	hex_table['E'] = 14;
	hex_table['F'] = 15;
	hex_table['a'] = 10;
	hex_table['b'] = 11;
	hex_table['c'] = 12;
	hex_table['d'] = 13;
	hex_table['e'] = 14;
	hex_table['f'] = 15;

	/* delimiters of significance are flagged w/ negative value */
	hex_table[' '] = -1;
	hex_table[','] = -1;
	hex_table['}'] = -1;
	hex_table['\n'] = -1;
	hex_table['\t'] = -1;

	initialized = true;
}

static int
next_int (jio::BufferedReader *stream)
{
	int ch;
	int value = 0;
	int gotone = 0;
	int done = 0;
    
	while (!done) {
		ch = stream->Read();
		if (ch < 0) {
			value = -1;
			done++;
		} else {
			ch &= 0xff;
			if (isascii (ch) && isxdigit (ch)) {
				value = (value << 4) + hex_table[ch];
				gotone++;
			} else if ((hex_table[ch]) < 0 && gotone) {
				done++;
			}
		}
	}
	return value;
}

cairo_surface_t * create_xbm_surface_from_stream(jio::InputStream *stream) 
{
	uint8_t *bits = nullptr;		// working variable 
	int size = 1;						// number of bytes of data 
	char name_and_type[MAX_SIZE];	// an input line 
	char *type;							// for parsing 
	int value;							// from an input line 
	int version10p;					// boolean, old format 
	int padding;						// to handle alignment 
	int bytes_per_line;			// per scanline of data 
	uint32_t ww = 0;				// width 
	uint32_t hh = 0;				// height 

	// first time initialization 
	if (!initialized) {
		init_hex_table();
	}

	jio::BufferedReader reader(stream);

	while (reader.IsEOF() == false) {
		std::string line = reader.ReadLine();

		if (line.size() == MAX_SIZE-1) {
			if (bits != nullptr) {
				delete [] bits;
				bits = nullptr;
			}

			return nullptr;
		}

		if (sscanf (line.c_str(), "#define %s %d", name_and_type, &value) == 2) {
			if (!(type = strrchr (name_and_type, '_')))
				type = name_and_type;
			else {
				type++;
			}

			if (!strcmp ("width", type))
				ww = (unsigned int) value;
			if (!strcmp ("height", type))
				hh = (unsigned int) value;
			if (!strcmp ("hot", type)) {
				if (type-- == name_and_type || type-- == name_and_type)
					continue;
			}

			continue;
		}
    
		if (sscanf (line.c_str(), "static short %s = {", name_and_type) == 1)
			version10p = 1;
		else if (sscanf (line.c_str(),"static unsigned char %s = {",name_and_type) == 1)
			version10p = 0;
		else if (sscanf (line.c_str(), "static char %s = {", name_and_type) == 1)
			version10p = 0;
		else
			continue;

		if (!(type = strrchr (name_and_type, '_')))
			type = name_and_type;
		else
			type++;

		if (strcmp ("bits[]", type))
			continue;
    
		if (!ww || !hh) {
			if (bits != nullptr) {
				delete [] bits;
				bits = nullptr;
			}

			return nullptr;
		}

		if ((ww % 16) && ((ww % 16) < 9) && version10p)
			padding = 1;
		else
			padding = 0;

		bytes_per_line = (ww+7)/8 + padding;

		if (bits != nullptr) {
			delete [] bits;
			bits = nullptr;
		}

		size = bytes_per_line * hh;
		bits = new uint8_t[size];

		if (version10p) {
			uint8_t *ptr = bits;

			for (int i=0; i<size; i+=2) {
				if ((value = next_int(&reader)) < 0) {
					// break;
				}
				
				*(ptr++) = value;
				
				if (!padding || ((i+2) % bytes_per_line)) {
					*(ptr++) = value >> 8;
				}
			}
		} else {
			uint8_t *ptr = bits;

			for (int i=0; i<size; i++) {
				if ((value = next_int(&reader)) < 0) {
					// break;
				}
				
				*ptr++ = value;
			}
		}
	}

	if (bits == nullptr) {
		return nullptr;
	}

	int sw = ww;
	int sh = hh;
	int sz = sw*sh;

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
		return nullptr;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == nullptr) {
		return nullptr;
	}

	uint8_t *src = bits;
	uint32_t *dst = (uint32_t *)data;
	uint8_t mask = 0x01;

	for (int i=0; i<sz; i++) {
		uint8_t bit = *src & mask;

		if (bit == 0x00) {
			dst[i] = 0xffffffff;
		} else {
			dst[i] = 0xff000000;
		}

		mask = mask << 0x01;

		if (mask == 0x00) {
			src = src + 1;
			mask = 0x01;
		}
	}

	delete [] bits;

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_xbm_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = nullptr;

	surface = create_xbm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_xbm_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const uint8_t *)data, size);

	cairo_surface_t *surface = nullptr;

	surface = create_xbm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}

