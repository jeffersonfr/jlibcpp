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
#ifndef J_IMAGE_H
#define J_IMAGE_H

#include "jgraphics.h"
#include "jfile.h"
#include "jinputstream.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

enum jpixelformat_t {
	JPF_UNKNOWN,	// 0x00000000	unknown or unspecified format
	JPF_ARGB1555,	// SURFACE_PIXELFORMAT(0, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0)  	16 bit ARGB (2 byte, alpha 1@15, red 5@10, green 5@5, blue 5@0)
	JPF_RGB16,		// SURFACE_PIXELFORMAT(1, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit RGB (2 byte, red 5@11, green 6@5, blue 5@0)
	JPF_RGB24,		// SURFACE_PIXELFORMAT(2, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)  	24 bit RGB (3 byte, red 8@16, green 8@8, blue 8@0)
	JPF_RGB32,		// SURFACE_PIXELFORMAT(3, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0)  	24 bit RGB (4 byte, nothing@24, red 8@16, green 8@8, blue 8@0)
	JPF_ARGB,			// SURFACE_PIXELFORMAT(4, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0)  	32 bit ARGB (4 byte, alpha 8@24, red 8@16, green 8@8, blue 8@0)
	JPF_A8,				// SURFACE_PIXELFORMAT(5, 0, 8, 1, 0, 1, 0, 0, 0, 0, 0)   	8 bit alpha (1 byte, alpha 8@0), e.g. anti-aliased glyphs
	JPF_YUY2,			// SURFACE_PIXELFORMAT(6, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit YUV (4 byte/ 2 pixel, macropixel contains CbYCrY [31:0])
	JPF_RGB332,		// SURFACE_PIXELFORMAT(7, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0)   	8 bit RGB (1 byte, red 3@5, green 3@2, blue 2@0)
	JPF_UYVY,			// SURFACE_PIXELFORMAT(8, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit YUV (4 byte/ 2 pixel, macropixel contains YCbYCr [31:0])
	JPF_I420,			// SURFACE_PIXELFORMAT(9, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)  	12 bit YUV (8 bit Y plane followed by 8 bit quarter size U/V planes)
	JPF_YV12,			// SURFACE_PIXELFORMAT(10, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by 8 bit quarter size V/U planes)
	JPF_LUT8,			// SURFACE_PIXELFORMAT(11, 8, 0, 1, 0, 1, 0, 0, 0, 1, 0)  	8 bit LUT (8 bit color and alpha lookup from palette)
	JPF_ALUT44,		// SURFACE_PIXELFORMAT(12, 4, 4, 1, 0, 1, 0, 0, 0, 1, 0)  	8 bit ALUT (1 byte, alpha 4@4, color lookup 4@0)
	JPF_AiRGB,		// SURFACE_PIXELFORMAT(13, 24, 8, 1, 0, 4, 0, 0, 0, 0, 1) 	32 bit ARGB (4 byte, inv. alpha 8@24, red 8@16, green 8@8, blue 8@0)
	JPF_A1,				// SURFACE_PIXELFORMAT(14, 0, 1, 1, 1, 0, 7, 0, 0, 0, 0)  	1 bit alpha (1 byte/ 8 pixel, most significant bit used first)
	JPF_NV12,			// SURFACE_PIXELFORMAT(15, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cb|Cr [7:0|7:0] plane)
	JPF_NV16,			// SURFACE_PIXELFORMAT(16, 24, 0, 0, 0, 1, 0, 0, 1, 0, 0) 	16 bit YUV (8 bit Y plane followed by one 16 bit half width Cb|Cr [7:0|7:0] plane)
	JPF_ARGB2554,	// SURFACE_PIXELFORMAT(17, 14, 2, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit ARGB (2 byte, alpha 2@14, red 5@9, green 5@4, blue 4@0)
	JPF_ARGB4444,	// SURFACE_PIXELFORMAT(18, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit ARGB (2 byte, alpha 4@12, red 4@8, green 4@4, blue 4@0)
	JPF_RGBA4444,	// SURFACE_PIXELFORMAT(19, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGBA (2 byte, red 4@12, green 4@8, blue 4@4, alpha 4@0)
	JPF_NV21,			// SURFACE_PIXELFORMAT(20, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cr|Cb [7:0|7:0] plane)
	JPF_AYUV,			// SURFACE_PIXELFORMAT(21, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0) 	32 bit AYUV (4 byte, alpha 8@24, Y 8@16, Cb 8@8, Cr 8@0)
	JPF_A4,				// SURFACE_PIXELFORMAT(22, 0, 4, 1, 4, 0, 1, 0, 0, 0, 0)  	4 bit alpha (1 byte/ 2 pixel, more significant nibble used first)
	JPF_ARGB1666,	// SURFACE_PIXELFORMAT(23, 18, 1, 1, 0, 3, 0, 0, 0, 0, 0) 	1 bit alpha (3 byte/ alpha 1@18, red 6@12, green 6@6, blue 6@0)
	JPF_ARGB6666,	// SURFACE_PIXELFORMAT(24, 18, 6, 1, 0, 3, 0, 0, 0, 0, 0) 	6 bit alpha (3 byte/ alpha 6@18, red 6@12, green 6@6, blue 6@0)
	JPF_RGB18,		// SURFACE_PIXELFORMAT(25, 18, 0, 0, 0, 3, 0, 0, 0, 0, 0) 	6 bit RGB (3 byte/ red 6@12, green 6@6, blue 6@0)
	JPF_LUT2,			// SURFACE_PIXELFORMAT(26, 2, 0, 1, 2, 0, 3, 0, 0, 1, 0)  	2 bit LUT (1 byte/ 4 pixel, 2 bit color and alpha lookup from palette)
	JPF_RGB444,		// SURFACE_PIXELFORMAT(27, 12, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGB (2 byte, nothing @12, red 4@8, green 4@4, blue 4@0)
	JPF_RGB555,		// SURFACE_PIXELFORMAT(28, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGB (2 byte, nothing @15, red 5@10, green 5@5, blue 5@0)
	JPF_BGR555,		// SURFACE_PIXELFORMAT(29, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0)
	JPF_RGBA5551,	// SURFACE_PIXELFORMAT(30, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGBA (2 byte, red 5@11, green 5@6, blue 5@1, alpha 1@0)
	JPF_AVYU,			// SURFACE_PIXELFORMAT(31, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0) 	32 bit AVYU 4:4:4 (4 byte, alpha 8@24, Cr 8@16, Y 8@8, Cb 8@0)
	JPF_VYU,			// SURFACE_PIXELFORMAT(32, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)
};

enum jflip_flags_t {
	JFF_HORIZONTAL = 0x01,
	JFF_VERTICAL = 0x02
};

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Image : public virtual jcommon::Object{

	protected:
		/** \brief */
		Graphics *_graphics;
		/** \brief */
		struct jsize_t _size;
		/** \brief */
		jpixelformat_t _pixelformat;

	protected:
		/**
		 * \brief
		 *
		 */
		Image(jpixelformat_t pixelformat, int wp, int hp);
		
	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Image();

		/**
		 * \brief
		 *
		 */
		static jsize_t GetImageSize(std::string img);
		
		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(jpixelformat_t pixelformat, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(uint32_t *data, int wp, int hp);

		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(char *data, int size);

		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(std::string image);
		
		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(jio::File *file);

		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(jio::InputStream *stream);

		/**
		 * \brief
		 *
		 */
		static Image * CreateImage(Image *image);

		/**
		 * \brief
		 *
		 */
		virtual Graphics * GetGraphics();

		/**
		 * \brief
		 *
		 */
		virtual jpixelformat_t GetPixelFormat();

		/**
		 * \brief
		 *
		 */
		virtual int GetWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHeight();

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetSize();
		
		/**
		 * \brief
		 *
		 */
		virtual Image * Flip(jflip_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual Image * Rotate(double radians, bool resize = true);
		
		/**
		 * \brief
		 *
		 */
		virtual Image * Scale(int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual Image * Crop(int xp, int yp, int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual Image * Blend(double alpha);

		/**
		 * \brief
		 *
		 */
		virtual Image * Colorize(Color color);

		/**
		 * \brief
		 *
		 */
		virtual void SetPixels(uint8_t *rgb, int xp, int yp, int wp, int hp, int stride);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetPixels(uint8_t **rgb, int xp, int yp, int wp, int hp, int *stride);
	
		/**
		 * \brief
		 *
		 */
		virtual void GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();
		
		/**
		 * \brief
		 *
		 */
		virtual void Restore();
		
		/**
		 * \brief
		 *
		 */
		virtual jcommon::Object * Clone();

};

}

#endif 
