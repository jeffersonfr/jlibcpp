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
/*
   (c) Copyright 2012-2013  DirectFB integrated media GmbH
   (c) Copyright 2001-2013  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Shimokawa <andi@directfb.org>,
              Marek Pikarski <mass@directfb.org>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "Stdafx.h"
#include "genericprovider_gif.h"
#include "jfileinputstream.h"
#include "jmemoryinputstream.h"
#include "jdebug.h"

#define MAXCOLORMAPSIZE 256

#define CM_RED   0
#define CM_GREEN 1
#define CM_BLUE  2

#define MAX_LWZ_BITS 12

#define INTERLACE     0x40
#define LOCALCOLORMAP 0x80

#define BitSet(byte, bit) (((byte) & (bit)) == (bit))

#define LM_to_uint(a,b) (((b)<<8)|(a))

namespace jgui {

struct GIFData {
     uint32_t *image;
     int image_width;
     int image_height;
     bool image_transparency;
     uint32_t image_colorkey;

     uint32_t Width;
     uint32_t Height;
     uint8_t ColorMap[3][MAXCOLORMAPSIZE];
     uint32_t BitPixel;
     uint32_t ColorResolution;
     uint32_t Background;
     uint32_t AspectRatio;

     int GrayScale;
     int transparent;
     int delayTime;
     int inputFlag;
     int disposal;

     uint8_t buf[280];
     int curbit;
		 int lastbit;
		 int done;
		 int last_byte;

     int fresh;
     int code_size;
		 int set_code_size;
     int max_code;
		 int max_code_size;
     int firstcode;
		 int oldcode;
     int clear_code;
		 int end_code;
     int table[2][(1<< MAX_LWZ_BITS)];
     int stack[(1<<(MAX_LWZ_BITS))*2], *sp;

		 jio::InputStream *stream;
};

static int ReadColorMap(jio::InputStream *stream, int number, uint8_t buf[3][MAXCOLORMAPSIZE])
{
	uint8_t rgb[3];

	for (int i=0; i<number; i++) {
		if (stream->Read((char *)rgb, sizeof(rgb)) <= 0) {
			JDEBUG(JINFO, "bad colormap" );

			return true;
		}

		buf[CM_RED][i] = rgb[0];
		buf[CM_GREEN][i] = rgb[1];
		buf[CM_BLUE][i] = rgb[2];
	}

	return false;
}

static int GetDataBlock(jio::InputStream *stream, uint8_t *buf)
{
	uint8_t count;

	if (stream->Read((char *)&count, 1) <= 0) {
		JDEBUG(JINFO, "error in getting DataBlock size" );

		return -1;
	}

	if (count == 0) {
		return -1;
	}

	if (count != 0 && stream->Read((char *)buf, count) <= 0) {
		JDEBUG(JINFO, "error in reading DataBlock" );

		return -1;
	}

	return count;
}

static int GetCode(GIFData *data, int code_size, int flag)
{
	int i, j, ret;
	uint8_t count;

	if (flag) {
		data->curbit = 0;
		data->lastbit = 0;
		data->done = false;

		return 0;
	}

	if ( (data->curbit+code_size) >= data->lastbit) {
		if (data->done) {
			if (data->curbit >= data->lastbit) {
				JDEBUG(JINFO, "ran off the end of my bits" );
			}

			return -1;
		}

		data->buf[0] = data->buf[data->last_byte-2];
		data->buf[1] = data->buf[data->last_byte-1];

		if ((count = GetDataBlock(data->stream, &data->buf[2] )) == 0) {
			data->done = true;
		}

		data->last_byte = 2 + count;
		data->curbit = (data->curbit - data->lastbit) + 16;
		data->lastbit = (2+count) * 8;
	}

	ret = 0;

	for (i = data->curbit, j = 0; j < code_size; ++i, ++j) {
		ret |= ((data->buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;
	}

	data->curbit += code_size;

	return ret;
}

static int DoExtension(GIFData *data, int label)
{
	uint8_t buf[256] = { 0 };
	char *str;

	switch (label) {
		case 0x01:              // Plain Text Extension
			str = (char *)"Plain Text Extension";
			break;
		case 0xff:              // Application Extension
			str = (char *)"Application Extension";
			break;
		case 0xfe:              // Comment Extension
			str = (char *)"Comment Extension";

			while (GetDataBlock(data->stream, (uint8_t*)buf ) > 0);
			
			return false;
		case 0xf9:              // Graphic Control Extension
			str = (char *)"Graphic Control Extension";

			(void)GetDataBlock(data->stream, (uint8_t*)buf);

			data->disposal = (buf[0] >> 2) & 0x7;
			data->inputFlag = (buf[0] >> 1) & 0x1;
			data->delayTime = LM_to_uint(buf[1], buf[2]);

			if ((buf[0] & 0x1) != 0) {
				data->transparent = buf[3];
			}

			while (GetDataBlock(data->stream, (uint8_t*) buf) > 0);

			return false;
		default:
			str = (char*) buf;
			snprintf(str, 256, "UNKNOWN (0x%02x)", label);
			break;
	}

	while (GetDataBlock(data->stream, (uint8_t*) buf) > 0);

	return false;
}

static int LWZReadByte(GIFData *data, int flag, int input_code_size)
{
	int i, code, incode;

	if (flag) {
		data->set_code_size = input_code_size;
		data->code_size = data->set_code_size+1;
		data->clear_code = 1 << data->set_code_size ;
		data->end_code = data->clear_code + 1;
		data->max_code_size = 2*data->clear_code;
		data->max_code = data->clear_code+2;

		GetCode(data, 0, true);

		data->fresh = true;

		for (i = 0; i < data->clear_code; ++i) {
			data->table[0][i] = 0;
			data->table[1][i] = i;
		}
		
		for (; i < (1<<MAX_LWZ_BITS); ++i) {
			data->table[0][i] = data->table[1][0] = 0;
		}
		
		data->sp = data->stack;

		return 0;
	} else if (data->fresh) {
		data->fresh = false;

		do {
			data->firstcode = data->oldcode = GetCode( data, data->code_size, false );
		} while (data->firstcode == data->clear_code);

		return data->firstcode;
	}

	if (data->sp > data->stack) {
		return *--data->sp;
	}

	while ((code = GetCode( data, data->code_size, false )) >= 0) {
		if (code == data->clear_code) {
			for (i = 0; i < data->clear_code; ++i) {
				data->table[0][i] = 0;
				data->table[1][i] = i;
			}
		
			for (; i < (1<<MAX_LWZ_BITS); ++i) {
				data->table[0][i] = data->table[1][i] = 0;
			}
			
			data->code_size = data->set_code_size+1;
			data->max_code_size = 2*data->clear_code;
			data->max_code = data->clear_code+2;
			data->sp = data->stack;
			data->firstcode = data->oldcode = GetCode( data, data->code_size, false );

			return data->firstcode;
		} else if (code == data->end_code) {
			int count;
			uint8_t buf[260];

			while ((count = GetDataBlock(data->stream, buf)) > 0);

			if (count != 0) {
				JDEBUG(JINFO, "missing EOD in data stream " "(common occurence)");
			}

			return -2;
		}

		incode = code;

		if (code >= data->max_code) {
			*data->sp++ = data->firstcode;
			code = data->oldcode;
		}

		while (code >= data->clear_code) {
			*data->sp++ = data->table[1][code];
			
			if (code == data->table[0][code]) {
				JDEBUG(JINFO, "circular table entry BIG ERROR");
			}
			
			code = data->table[0][code];
		}

		*data->sp++ = data->firstcode = data->table[1][code];

		if ((code = data->max_code) <(1<<MAX_LWZ_BITS)) {
			data->table[0][code] = data->oldcode;
			data->table[1][code] = data->firstcode;
			++data->max_code;
			
			if ((data->max_code >= data->max_code_size) && (data->max_code_size < (1<<MAX_LWZ_BITS))) {
				data->max_code_size *= 2;
				++data->code_size;
			}
		}

		data->oldcode = incode;

		if (data->sp > data->stack) {
			return *--data->sp;
		}
	}

	return code;
}

static int SortColors (const void *a, const void *b)
{
	return (*((const uint8_t *) a) - *((const uint8_t *) b));
}

// looks for a color that is not in the colormap and ideally not even close to the colors used in the colormap
static uint32_t FindColorKey(int n_colors, uint8_t cmap[3][MAXCOLORMAPSIZE])
{
	uint32_t color = 0xFF000000;
	uint8_t csort[MAXCOLORMAPSIZE];
	int i, j, index, d;

	if (n_colors < 1) {
		return color;
	}

	// D_ASSERT( n_colors <= MAXCOLORMAPSIZE );

	for (i = 0; i < 3; i++) {
		memcpy(csort, cmap[i], n_colors);
		qsort(csort, n_colors, 1, SortColors);

		for (j = 1, index = 0, d = 0; j < n_colors; j++) {
			if (csort[j] - csort[j-1] > d) {
				d = csort[j] - csort[j-1];
				index = j;
			}
		}
		
		if ((csort[0] - 0x0) > d) {
			d = csort[0] - 0x0;
			index = n_colors;
		}
		
		if (0xFF - (csort[n_colors - 1]) > d) {
			index = n_colors + 1;
		}

		if (index < n_colors) {
			csort[0] = csort[index] - (d/2);
		} else if (index == n_colors) {
			csort[0] = 0x0;
		} else {
			csort[0] = 0xFF;
		}

		color |= (csort[0] << (8 * (2 - i)));
	}

	return color;
}

static uint32_t * ReadImage(GIFData *data, int width, int height, uint8_t cmap[3][MAXCOLORMAPSIZE], uint32_t key_rgb, bool interlace, bool ignore)
{
	int v, xpos = 0, ypos = 0, pass = 0;
	uint32_t *image;
	uint8_t c;

	// Initialize the decompression routines
	if (data->stream->Read((char *)&c, 1 ) <= 0) {
		JDEBUG(JINFO, "EOF / read error on image data" );
	}

	if (LWZReadByte(data, true, c) < 0) {
		JDEBUG(JINFO, "error reading image" );
	}

	// If this is an "uninteresting picture" ignore it.
	if (ignore) {
		while (LWZReadByte(data, false, c) >= 0);

		return NULL;
	}

	// FIXME: 
	// allocates four additional bytes because the scaling functions in src/misc/gfx_util.c 
	// have an off-by-one bug which causes segfaults on darwin/osx (not on linux)
	image = new uint32_t[width * height + 4];

	while ((v = LWZReadByte( data, false, c )) >= 0) {
		uint32_t *dst = image + (ypos * width + xpos);

		if (v == data->transparent) {
			*dst++ = key_rgb;
		} else {
			*dst++ = (0xff000000 | cmap[CM_RED][v] << 16 | cmap[CM_GREEN][v] << 8  | cmap[CM_BLUE][v]);
		}

		++xpos;

		if (xpos == width) {
			xpos = 0;

			if (interlace) {
				switch (pass) {
					case 0:
					case 1:
						ypos += 8;
						break;
					case 2:
						ypos += 4;
						break;
					case 3:
						ypos += 2;
						break;
				}

				if (ypos >= height) {
					++pass;

					switch (pass) {
						case 1:
							ypos = 4;
							break;
						case 2:
							ypos = 2;
							break;
						case 3:
							ypos = 1;
							break;
						default:
							goto fini;
					}
				}
			} else {
				++ypos;
			}
		}

		if (ypos >= height) {
			break;
		}
	}

fini:
	if (LWZReadByte( data, false, c ) >= 0) {
		JDEBUG(JINFO, "too much input data, ignoring extra...");
	}

	return image;
}

static uint32_t * ReadGIF(GIFData *data, int imageNumber, int *width, int *height, bool *transparency, uint32_t *key_rgb, bool alpha, bool headeronly)
{
	uint32_t colorKey = 0;
	int bitPixel;
	int imageCount = 0;
	char version[4];
	uint8_t buf[16];
	uint8_t c;
	uint8_t localColorMap[3][MAXCOLORMAPSIZE];
	bool useGlobalColormap;

	if (data->stream->Read((char *)buf, 6) <= 0) {
		JDEBUG(JINFO, "error reading magic number" );
	}

	if (strncmp( (char *)buf, "GIF", 3 ) != 0) {
		JDEBUG(JINFO, "not a GIF file" );
	}

	memcpy(version, (char *)buf + 3, 4);

	if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0)) {
		JDEBUG(JINFO, "bad version number, not '87a' or '89a'" );
	}

	if (data->stream->Read((char *)buf, 7) <= 0) {
		JDEBUG(JINFO, "failed to read screen descriptor" );
	}

	data->Width = LM_to_uint( buf[0], buf[1] );
	data->Height = LM_to_uint( buf[2], buf[3] );
	data->BitPixel = 2 << (buf[4] & 0x07);
	data->ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
	data->Background = buf[5];
	data->AspectRatio = buf[6];

	// Global Colormap
	if (BitSet(buf[4], LOCALCOLORMAP)) {
		if (ReadColorMap(data->stream, data->BitPixel, data->ColorMap )) {
			JDEBUG(JINFO, "error reading global colormap" );
		}
	}

	if (data->AspectRatio != 0 && data->AspectRatio != 49) {
		// float r = ( (float) data->AspectRatio + 15.0 ) / 64.0;
		JDEBUG(JINFO, "warning - non-square pixels");
	}

	data->transparent = -1;
	data->delayTime = -1;
	data->inputFlag = -1;
	data->disposal = 0;

	for (;;) {
		if (data->stream->Read((char *)&c, 1) <= 0) {
			JDEBUG(JINFO, "EOF / read error on image data" );

			return NULL;
		}

		// GIF terminator
		if (c == ';') {
			if (imageCount < imageNumber) {
				JDEBUG(JINFO, "only %d image%s found in file", imageCount, imageCount>1?"s":"" );
			}

			return NULL;
		}

		// Extension
		if (c == '!') {
			if (data->stream->Read((char *)&c, 1) <= 0) {
				JDEBUG(JINFO, "EOF / read error on extention function code");
			}

			DoExtension( data, c );
			
			continue;
		}

		// Not a valid start character
		if (c != ',') {
			JDEBUG(JINFO, "bogus character 0x%02x, ignoring", (int) c );
			
			continue;
		}

		++imageCount;

		if (data->stream->Read((char *)buf, 9) <= 0) {
			JDEBUG(JINFO, "couldn't read left/top/width/height");
		}

		*width  = LM_to_uint( buf[4], buf[5] );
		*height = LM_to_uint( buf[6], buf[7] );
		*transparency = (data->transparent != -1);

		if (headeronly && !(*transparency && key_rgb)) {
			return NULL;
		}

		useGlobalColormap = ! BitSet( buf[8], LOCALCOLORMAP );

		if (useGlobalColormap) {
			if (*transparency && (key_rgb || !headeronly)) {
				colorKey = FindColorKey( data->BitPixel, data->ColorMap );
			}
		} else {
			bitPixel = 2 << (buf[8] & 0x07);
			
			if (ReadColorMap(data->stream, bitPixel, localColorMap)) {
				JDEBUG(JINFO, "error reading local colormap" );
			}

			if (*transparency && (key_rgb || !headeronly)) {
				colorKey = FindColorKey( bitPixel, localColorMap );
			}
		}

		if (key_rgb) {
			*key_rgb = colorKey;
		}

		if (headeronly) {
			return NULL;
		}

		if (alpha) {
			colorKey &= 0x00FFFFFF;
		}

		return ReadImage(data, *width, *height, 
				(useGlobalColormap?data->ColorMap:localColorMap), colorKey, BitSet( buf[8], INTERLACE), imageCount != imageNumber);
	}
}

cairo_surface_t * create_gif_surface_from_stream(jio::InputStream *stream) 
{
	if (stream == NULL) {
		return NULL;
	}

	GIFData t;

	t.stream = stream;
	t.image = NULL;
	t.image_width = -1;
	t.image_height = -1;
	t.image_transparency = false;
	t.image_colorkey = 0x00000000;
	t.Width = 0;
	t.Height = 0;
	t.BitPixel = 0;
	t.ColorResolution = 0;
	t.Background = 0;
	t.AspectRatio = 0;
	t.GrayScale = 0;
	t.transparent = 0;
	t.delayTime = 0;
	t.inputFlag = 0;
	t.disposal = 0;
	t.curbit = 0;
	t.lastbit = 0;
	t.done = 0;
	t.last_byte = 0;
	t.fresh = 0;
	t.code_size = 0;
	t.set_code_size = 0;
	t.max_code = 0;
	t.max_code_size = 0;
	t.firstcode = 0;
	t.oldcode = 0;
	t.clear_code = 0;
	t.end_code = 0;
	t.stream = stream;

	t.image = ReadGIF(&t, 1, &t.image_width, &t.image_height, &t.image_transparency, &t.image_colorkey, true, false);

	if (t.image == NULL || (t.image_height == 0) || (t.image_width  == 0)) {
		return NULL;
	}

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, t.image_width, t.image_height);

	if (surface == NULL) {
		return NULL;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == NULL) {
		delete [] t.image;

		return NULL;
	}

	memcpy(data, t.image, t.image_width*t.image_height*4);

	delete [] t.image;

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_gif_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = NULL;

	surface = create_gif_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_gif_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const char *)data, size);

	cairo_surface_t *surface = NULL;

	surface = create_gif_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}

