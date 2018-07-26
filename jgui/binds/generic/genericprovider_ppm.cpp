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
#include "generic/include/genericprovider_ppm.h"
#include "jgui/jgraphics.h"
#include "jio/jfileinputstream.h"
#include "jio/jmemoryinputstream.h"

#include <string.h>

#define P_GET( buf, n ) \
{\
	len = data->stream->Read((char *)buf, n);\
	if (len == 0) {\
		return 0;\
	} else if (len < 0) {\
		return -1;\
	}\
}

#define P_LOADBUF() \
{\
	int size = data->chunksize * data->width;\
	if (data->bufp) {\
		size -= data->bufp;\
		memset( data->rowbuf + data->bufp, 0, size + 1 );\
		P_GET( data->rowbuf + data->bufp, size );\
		len += data->bufp;\
		data->bufp = 0;\
	} else {\
		memset( data->rowbuf, 0, size + 1 );\
		P_GET( data->rowbuf, size );\
	}\
}

#define P_STOREBUF() \
{\
	int size = data->chunksize * data->width;\
	if (i++ < (int)len && i < (int)size) {\
		size -= i;\
		memcpy( data->rowbuf, data->rowbuf + i, size );\
		data->bufp = size;\
	}\
}

namespace jgui {

enum PHeader {
	PHDR_MAGIC,
	PHDR_WIDTH,
	PHDR_HEIGHT,
	PHDR_COLORS
};

enum PFormat {
	PFMT_PBM = 0,
	PFMT_PGM = 1,
	PFMT_PPM = 2
};

enum PImgType {
	PIMG_RAW   = 0,
	PIMG_PLAIN = 1
};

typedef int (*PRowCallback)(void *data, uint8_t *dest);

struct PFormatData {
	PRowCallback callback;
	int chunksize;
};

struct PNMData {
	PFormat         format;
	PImgType        type;

	int             width;
	int             height;
	int             colors;

	PRowCallback    getrow;
	uint8_t         *rowbuf;   // buffer for ascii images
	int             bufp;      // current position in buffer
	int             chunksize; // maximum size of each sample

	jio::InputStream *stream;
};

static int __rawpbm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	int  len;
	int           i, j;
	uint8_t       *s = dest;
	uint32_t      *d = (uint32_t*) dest;

	P_GET( dest, data->width / 8 );

	// start from end
	for (i = (len * 8), j = 0; --i >= 0; ) {
		d[i] = (s[i >> 3] & (1 << j)) ? 0x00000000  /* alpha:0x00, color:black */ : 0xffffffff; /* alpha:0xff, color:white */

		if (++j > 7) {
			j = 0;
		}
	}

	return 0;
}

static int __rawpgm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	int  len;
	uint8_t       *s = dest;
	uint32_t      *d = (uint32_t*) dest;

	P_GET( dest, data->width );

	// start from end
	while (--len >= 0) {
		int p = s[len];

		d[len] = (0xff << 0x18) | (p << 0x10) | (p << 0x08) | (p << 0x00);
	}

	return 0;
}

static int __rawppm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	int  len;
	int           i;
	uint8_t       *s = dest;
	uint32_t      *d = (uint32_t*) dest;

	P_GET( dest, data->width * 3 );

	// start from end
	for (i = len/3; --i >= 0;) {
		int r = s[i*3+0];
		int g = s[i*3+1];
		int b = s[i*3+2];

		d[i] = (0xff << 0x18) | (r << 0x10) | (g << 0x08) | (b << 0x00);
	}

	return 0;
}

static int __plainpbm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	unsigned int  len;
	int           i;
	int           w = data->width;
	uint8_t       *buf  = data->rowbuf;
	uint32_t      *d = (uint32_t*) dest;

	P_LOADBUF();

	for (i = 0; i < (int)len; i++) {
		if (buf[i] == 0) {
			break;
		}

		switch (buf[i]) {
			case '0':
				*d++ = 0xffffffff; // alpha:0xff, color:white
				break;
			case '1':
				*d++ = 0x00000000; // alpha:0x00, color:black
				break;
			default:
				continue;
		}

		// assume next char is a space
		i++;

		if (!--w) {
			break;
		}
	}

	P_STOREBUF();

	return 0;
}

static int __plainpgm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	unsigned int  len;
	int           i, n;
	int           w = data->width;
	uint8_t       *buf = data->rowbuf;
	uint32_t      *d = (uint32_t*) dest;

	P_LOADBUF();

	for (i = 0, n = 0; i < (int)len; i++) {
		if (buf[i] == 0) {
			break;
		}

		if (buf[i] < '0' || buf[i] > '9') {
			n = 0;
			continue;
		}

		n *= 10;
		n += buf[i] - '0';

		if (isspace( buf[i+1] )) {
			*d++ = (0xff << 0x18) | (n << 0x10) | (n << 0x08) | (n << 0x00);
			n = 0;
			i++;

			if (!--w) {
				break;
			}

			*d = 0;
		}
	}

	P_STOREBUF();

	return 0;
}

static int __plainppm_getrow(void *ptr, uint8_t *dest)
{
	PNMData *data = (PNMData *)ptr;
	unsigned int  len;
	int           i, n;
	int           j = 16;
	int           w = data->width;
	uint8_t       *buf = data->rowbuf;
	uint32_t      *d = (uint32_t*) dest;

	P_LOADBUF();

	for (i = 0, n = 0; i < (int)len; i++) {
		if (buf[i] == 0) {
			break;
		}

		if (buf[i] < '0' || buf[i] > '9') {
			n = 0;

			continue;
		}

		n *= 10;
		n += buf[i] - '0';

		if (isspace( buf[i+1] )) {
			*d |= (n & 0xff) << j;
			n = 0;
			i++;
			j -= 8;

			if (j < 0) {
				*d++ |= 0xff000000;
				j = 16;

				if (!--w) {
					break;
				}

				*d = 0;
			}
		}
	}

	P_STOREBUF();

	return 0;
}

static const PFormatData p_dta[][2] = {
	{ {__rawpbm_getrow, 0}, {__plainpbm_getrow,  2} }, // PBM 
	{ {__rawpgm_getrow, 0}, {__plainpgm_getrow,  4} }, // PGM 
	{ {__rawppm_getrow, 0}, {__plainppm_getrow, 12} }  // PPM 
};

static int p_getheader(PNMData *data, char *to, int size)
{
	int len;

	while (size--) {
		P_GET( to, 1 );

		if (*to == '#') {
			char c = 0;

			*to = 0;

			while (c != '\n') {
				P_GET( &c, 1 );
			}

			return 0;
		} else if (isspace( *to )) {
			*to = 0;

			return 0;
		}

		to++;
	}

	return 0;
}

static int p_init(PNMData *data)
{
	PHeader   header  = PHDR_MAGIC;
	char      buf[33];
	int 			err;

	data->bufp = 0;
	data->width = -1;
	data->height = -1;

	memset( buf, 0, 33 );

	while ((err = p_getheader( data, buf, 32 )) == 0) {
		if (buf[0] == 0) {
			continue;
		}

		switch (header) {
			case PHDR_MAGIC: {
				if (buf[0] != 'P') {
					return 0;
				}
				
				switch (buf[1]) {
					case '1':
					case '4':
						data->format = PFMT_PBM;
						break;
					case '2':
					case '5':
						data->format = PFMT_PGM;
						break;
					case '3':
					case '6':
						data->format = PFMT_PPM;
						break;
					default:
						return -1;
				}

				data->type      = (buf[1] > '3') ? PIMG_RAW : PIMG_PLAIN;
				data->getrow    = p_dta[data->format][data->type].callback;
				data->chunksize = p_dta[data->format][data->type].chunksize;

				header = PHDR_WIDTH;
			} break;

			case PHDR_WIDTH: {
				data->width = strtol( buf, NULL, 10 );

				if (data->width < 1) {
					return -1;
				}

				if (data->format == PFMT_PBM && data->width & 7) {
					return -1;
				}

				header = PHDR_HEIGHT;
			} break;

			case PHDR_HEIGHT: {
				data->height = strtol( buf, NULL, 10 );

				if (data->height < 1) {
					return -1;
				}

				if (data->format == PFMT_PBM) {
					return 0;
				}

				header = PHDR_COLORS;
			} break;

			case PHDR_COLORS: {
				data->colors = strtoul( buf, NULL, 10 );

				if (data->colors < 1) {
					return -1;
				}

				if (data->colors > 0xff) {
					printf( "PNM: 2-bytes samples are not supported.\n" );

					return -1;
				}

				return 0;
			} break;
		}
	}

	return err;
}

cairo_surface_t * create_ppm_surface_from_stream(jio::InputStream *stream)
{
	cairo_surface_t *surface = NULL;
	
	PNMData pnm;
	
	pnm.width = -1;
	pnm.height = -1;
	pnm.colors = 0;
	pnm.getrow = NULL;
	pnm.rowbuf = NULL;
	pnm.bufp = 0;
	pnm.chunksize = 0;
	pnm.stream = stream;

	if (p_init(&pnm) < 0) {
		return NULL;
	}

	if (pnm.width < 0 || pnm.height < 0 || pnm.width > 16387 || pnm.height > 16387) {
		return NULL;
	}

	if (pnm.format == PFMT_PBM) {
		// A1
		return NULL;
	} else if (pnm.format == PFMT_PGM) {
		// A8
		return NULL;
	} else {
		// RGB32
	}

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, pnm.width, pnm.height);

	if (surface == NULL) {
		return NULL;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == NULL) {
		return NULL;
	}

	uint8_t row[pnm.width*4];
	uint8_t *ptr = data;

	pnm.rowbuf = new uint8_t[pnm.width*12];

	for (int y = 0; y < pnm.height; y++) {
		if (pnm.getrow( (void *)&pnm, (uint8_t *) row) != 0) {
			break;
		}

		for (int x = 0; x < pnm.width; x++) {
			ptr[3] = row[x*4+3];
			ptr[2] = row[x*4+2];
			ptr[1] = row[x*4+1];
			ptr[0] = row[x*4+0];

			ptr = ptr + 4;
		}
	}

	if (pnm.rowbuf) {
		delete [] pnm.rowbuf;
	}

	int sz = pnm.width*pnm.height;

	for (int i=0; i<sz; i++) {
		int alpha = data[i*4+3];
		data[i*4+2] = ALPHA_PREMULTIPLY(data[i*4+2], alpha);
		data[i*4+1] = ALPHA_PREMULTIPLY(data[i*4+1], alpha);
		data[i*4+0] = ALPHA_PREMULTIPLY(data[i*4+0], alpha);
	}

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_ppm_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = NULL;

	surface = create_ppm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_ppm_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const uint8_t *)data, size);

	cairo_surface_t *surface = NULL;

	surface = create_ppm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}

