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
/**
 * Copyright (c) 2008 Karlan Thomas Mitchell<karlanmitchell@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *         This product includes software developed by
           Karlan Thomas Mitchell<karlanmitchell@gmail.net>
 * 4. Neither Karlan Thomas Mitchell nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KARLAN THOMAS MITCHELL AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */ 
#include "providers/include/imageprovider_xpm.h"

#include "jio/jfileinputstream.h"
#include "jio/jmemoryinputstream.h"
#include "jio/jbufferedreader.h"

#include <map>

#include <string.h>

namespace jgui {

int command_decoder(char * cmd_string, size_t item, char * buffer, size_t buf_len)
{
	size_t cur = 0;
	size_t cmd_len = strlen(cmd_string);
	size_t cur_item = 1;
	int inside_quote = 0;
	int last_slash = 0;
	int last_space = 0;
	int exists = 0;
	char tbuf[2] = {0, 0};

	*buffer = 0;

	while (cur < cmd_len) {
		if (last_slash == 0) {
			if (last_space == 1 && cmd_string[cur] != ' ') {
				last_space = 0;
				cur_item++;
			}

			if (cmd_string[cur] == '\"') {
				(inside_quote == 1) ? (inside_quote = 0) : (inside_quote = 1);
			} else if (cmd_string[cur] == '\'') {
				(inside_quote == 1) ? (inside_quote = 0) : (inside_quote = 1);
			} else if (cmd_string[cur] == '\\') {
				last_slash = 1;
			} else if (inside_quote == 0 && cmd_string[cur] == ' ') {
				last_space = 1;
			} else {
				*tbuf = cmd_string[cur];
			}
		} else {
			last_slash = 0;
			*tbuf = cmd_string[cur];
		}

		if (cur_item == item) {
			strncat(buffer, tbuf, buf_len);
			exists = 1;
		}

		*tbuf = 0;

		cur++;
	}

	return exists;
} 

static bool xpm_load(jio::InputStream *stream, uint8_t **image, int *width, int *height)
{
	std::map<std::string, uint32_t> palette;
	jio::BufferedReader reader(stream);
	std::string line_buffer;
	char buffer[4096];
	size_t linenum_tot = 0;
	size_t linenum_inf = 0;
	size_t colors = 0;
	size_t colors_tot = 0;
	size_t rows = 0;
	size_t rows_tot = 0;
	size_t columns_tot = 0;
	size_t ncolors = 0;
	size_t chars_per_pixel = 0;
	uint32_t *data = nullptr;

	while (reader.IsEOF() == false) {
		line_buffer = reader.ReadLine();
		linenum_tot++;

		if (line_buffer[0] == '"' ) {
			linenum_inf++;
			if (linenum_inf == 1) {
				line_buffer[0] = '0'; // for command parsing which are numbers
				command_decoder((char *)line_buffer.c_str(), 1, buffer, 1024);
				columns_tot = atoi(buffer);
				command_decoder((char *)line_buffer.c_str(), 2, buffer, 1024);
				rows_tot = atoi(buffer);
				command_decoder((char *)line_buffer.c_str(), 3, buffer, 1024);
				ncolors = atoi(buffer);
				colors_tot = ncolors;
				command_decoder((char *)line_buffer.c_str(), 4, buffer, 1024);
				chars_per_pixel = atoi(buffer);

				if (chars_per_pixel <= 0 || chars_per_pixel > 4) {
					return false;
				}

				if (columns_tot <= 0 || rows_tot <= 0) {
					return false;
				}

				if (columns_tot > 16536 || rows_tot > 16536) {
					return false;
				}

				*width = columns_tot;
				*height = rows_tot;

				data = new uint32_t[columns_tot*rows_tot*4];
			} else {
				command_decoder((char *)line_buffer.c_str(), 1, buffer, 4096);
				if (strlen(buffer) > 1)
					buffer[strlen(buffer) - 1] = 0;

				if (colors < colors_tot) { // load palette
					char code[3], color[256];
					char *ptr;

					code[0] = buffer[0];
					code[1] = buffer[1];
					code[2] = '\0';

					strncpy(color, buffer+6, 6);

					color[6] = '\0';

					palette[code] = strtol(color, &ptr, 16) | 0xff000000;

					colors = colors + 1;
				} else if (rows < rows_tot) { // load image data
					char code[chars_per_pixel+1];

					for (int i=0; i<(int)columns_tot; i++) {
						strncpy(code, buffer+i*chars_per_pixel, chars_per_pixel);

						code[chars_per_pixel] = '\0';
						data[rows*columns_tot + i] = palette[code];
					}

					rows = rows + 1;
				}
			}
		}
	}

	*image = (uint8_t *)data;

	return true;
}

cairo_surface_t * create_xpm_surface_from_stream(jio::InputStream *stream) 
{
	uint8_t *buffer = nullptr;
	int sw = 0;
	int sh = 0;

	if (xpm_load(stream, &buffer, &sw, &sh) == false) {
		return nullptr;
	}

	unsigned int sz = sw*sh;

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
		return nullptr;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == nullptr) {
		return nullptr;
	}

	memcpy(data, buffer, sz*4);

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_xpm_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = nullptr;

	surface = create_xpm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_xpm_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const uint8_t *)data, size);

	cairo_surface_t *surface = nullptr;

	surface = create_xpm_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}

