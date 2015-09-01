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
#include "genericprovider_jpg.h"

#include <jpeglib.h>

#include <setjmp.h>

namespace jgui {

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	my_error_mgr *myerr = (my_error_mgr *) cinfo->err;

	// INFO:: error message
	// (*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
}

static cairo_surface_t * cairocks_surface_from_jpeg_private(struct jpeg_decompress_struct *cinfo) 
{
	cairo_surface_t * surface = 0;
	uint8_t * data = 0;
	uint8_t * rgb = 0;

	jpeg_read_header(cinfo, TRUE);
	jpeg_start_decompress(cinfo);

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, cinfo->image_width, cinfo->image_height);
	data = cairo_image_surface_get_data(surface);
	rgb = (uint8_t*)(malloc(cinfo->output_width * cinfo->output_components));

	while (cinfo->output_scanline < cinfo->output_height) {
		int scanline = cinfo->output_scanline * cairo_image_surface_get_stride(surface);

		jpeg_read_scanlines(cinfo, &rgb, 1);

		for (int i=0; i<(int)cinfo->output_width; i++) {
			int offset = scanline + (i * 4);

			data[offset + 3] = 255;
			data[offset + 2] = rgb[(i * 3) + 0];
			data[offset + 1] = rgb[(i * 3) + 1];
			data[offset + 0] = rgb[(i * 3) + 2];
		}
	}

	free(rgb);

	jpeg_finish_decompress(cinfo);
	jpeg_destroy_decompress(cinfo);

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_jpg_surface_from_file(const char *file) 
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	cairo_surface_t *surface;
	FILE *infile;

	if ((infile = fopen(file, "rb")) == NULL) {
		return NULL;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);

		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);

	surface = cairocks_surface_from_jpeg_private(&cinfo);

	fclose(infile);

	if (surface == NULL) {
		return NULL;
	}

	cairo_format_t format = cairo_image_surface_get_format(surface);
	int sw = cairo_image_surface_get_width(surface);
	int sh = cairo_image_surface_get_height(surface);

	if (format == CAIRO_FORMAT_INVALID || sw <= 0 || sh <= 0) {
		cairo_surface_destroy(surface);

		return NULL;
	}

	return surface;
}

cairo_surface_t * create_jpg_surface_from_data(uint8_t *data, int size)
{
	cairo_surface_t *surface;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);

		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, data, size);

	surface = cairocks_surface_from_jpeg_private(&cinfo);

	if (surface == NULL) {
		return NULL;
	}

	cairo_format_t format = cairo_image_surface_get_format(surface);
	int sw = cairo_image_surface_get_width(surface);
	int sh = cairo_image_surface_get_height(surface);

	if (format == CAIRO_FORMAT_INVALID || sw <= 0 || sh <= 0) {
		cairo_surface_destroy(surface);

		return NULL;
	}

	return surface;
}

}

