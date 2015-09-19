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
#include "jcolorconversion.h"
#include "jgraphics.h"

namespace jmedia {

ColorConversion::ColorConversion():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmedia::ColorConversion");
}

ColorConversion::~ColorConversion()
{
}

void ColorConversion::GetRGB32FromGray(uint8_t **gray_array, uint32_t **rgb32_array, int width, int height)
{
	uint32_t size_1 = width*height;
	uint8_t *src = ((uint8_t *)(*gray_array));
	uint8_t *dst = ((uint8_t *)(*rgb32_array));

	for (uint32_t i=0; i<size_1; i++) {
		dst[0] = src[0];
		dst[1] = src[0];
		dst[2] = src[0];
		dst[3] = 0xff;

		src = src + 1;
		dst = dst + 4;
	}
}

void ColorConversion::GetRGB32FromPalette(uint8_t **color_array, uint32_t **palette_array, uint32_t **rgb32_array, int width, int height)
{
	uint32_t size_1 = width*height;
	uint8_t *src = ((uint8_t *)(*color_array));
	uint32_t *dst = ((uint32_t *)(*rgb32_array));
	uint32_t *palette = ((uint32_t *)(*palette_array));

	for (uint32_t i=0; i<size_1; i++) {
		dst[0] = palette[src[0]];

		src = src + 1;
		dst = dst + 1;
	}
}

void ColorConversion::GetRGB32FromRGB16(uint8_t **rgb24_array, uint32_t **rgb32_array, int width, int height)
{
	uint32_t size_1 = width*height;
	uint8_t *src = ((uint8_t *)(*rgb24_array));
	uint8_t *dst = ((uint8_t *)(*rgb32_array));

	for (uint32_t i=0; i<size_1; i++) {
		dst[0] = (src[1] << 3) & 0xf8;
		dst[1] = ((src[0] << 5) & 0xf8) | ((src[1] >> 5) & 0x07);
		dst[2] = src[0] & 0xf8;
		dst[3] = 0xff;

		src = src + 2;
		dst = dst + 4;
	}
}

void ColorConversion::GetRGB32FromRGB24(uint8_t **rgb24_array, uint32_t **rgb32_array, int width, int height)
{
	uint32_t size_1 = width*height;
	uint8_t *src = ((uint8_t *)(*rgb24_array));
	uint8_t *dst = ((uint8_t *)(*rgb32_array));

	for (uint32_t i=0; i<size_1; i++) {
		dst[0] = src[2];
		dst[1] = src[1];
		dst[2] = src[0];
		dst[3] = 0xff;

		src = src + 3;
		dst = dst + 4;
	}
}

void ColorConversion::GetRGB32FromYV12(uint8_t **y_array, uint8_t **u_array, uint8_t **v_array, uint32_t **rgb32_array, int width, int height)
{
	uint8_t *ybuf = (uint8_t *)*y_array;
	uint8_t *ubuf = (uint8_t *)*u_array;
	uint8_t *vbuf = (uint8_t *)*v_array;
	uint8_t *rgb = (uint8_t *)*rgb32_array;

	// CHANGE:: avoid segfault
	height = height - 1;

	int size = width*height;
	int width_2 = width/2;
	int i;

	int py = 0;
	int px = 0;

	for (i=0; i<size; i+=2) {
		int y, u, v;
		int C, D, E;

		// pixel 1
		y = ybuf[2*(py*width+px)+0];
		u = ubuf[py * width_2 + px];
		v = vbuf[py * width_2 + px];

		C = y - 16;
		D = u - 128;
		E = v - 128;			

		if (y == 0) {
			D = E = 0;
		}

		rgb[2] = CLAMP((298 * C + 409 * E + 128) >> 8, 0, 255);
		rgb[1] = CLAMP((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255);
		rgb[0] = CLAMP((298 * C + 516 * D + 128) >> 8, 0, 255);
		rgb[3] = 0xff;

		rgb = rgb + 4;

		// pixel 2
		y = ybuf[2*(py*width+px)+1];
		// u = ubuf[py * width_2 + px];
		// v = vbuf[py * width_2 + px];

		C = y - 16;
		// D = u - 128;
		// E = v - 128;			

		if (y == 0) {
			D = E = 0;
		}

		rgb[2] = CLAMP((298 * C + 409 * E + 128) >> 8, 0, 255);
		rgb[1] = CLAMP((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255);
		rgb[0] = CLAMP((298 * C + 516 * D + 128) >> 8, 0, 255);
		rgb[3] = 0xff;

		rgb = rgb + 4;

		// update counters
		px = (px + 1)%width;

		if (px == 0) {
			py = py + 1;
		}
	}
}

void ColorConversion::GetRGB32FromYUYV(uint8_t **yuv_array, uint32_t **rgb32_array, int width, int height)
{
	uint8_t *pixel = ((uint8_t *)(*yuv_array));

	height = height - 1;

	uint32_t size_1 = width*height,
					 size_2 = size_1/2,
					 *ptr = *rgb32_array;
	int y[2], 
			u, 
			v;

	for (uint32_t j=0; j<size_2; j++) {
		y[0] = *(pixel+0);
		u = *(pixel+1);
		y[1] = *(pixel+2);
		v = *(pixel+3);

		uint8_t *argb;
		int C = 0;
		int D = u - 128;
		int E = v - 128;			

		// pixel 1
		argb = (uint8_t *)(ptr++);

		C = y[0] - 16;

		argb[2] = CLAMP((298 * C + 409 * E + 128) >> 8, 0, 255);
		argb[1] = CLAMP((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255);
		argb[0] = CLAMP((298 * C + 516 * D + 128) >> 8, 0, 255);
		argb[3] = 0xff;

		// pixel 2
		argb = (uint8_t *)(ptr++);

		C = y[1] - 16;

		argb[2] = CLAMP((298 * C + 409 * E + 128) >> 8, 0, 255);
		argb[1] = CLAMP((298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255);
		argb[0] = CLAMP((298 * C + 516 * D + 128) >> 8, 0, 255);
		argb[3] = 0xff;

		pixel = pixel + 4;
	}
}

}

