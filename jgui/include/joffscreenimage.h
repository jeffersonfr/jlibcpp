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
#ifndef J_OFFSCREENIMAGE_H
#define J_OFFSCREENIMAGE_H

#include "jgraphics.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

enum jsurface_pixelformat_t {
	SPF_UNKNOWN,	// 0x00000000	unknown or unspecified format
	SPF_ARGB1555,	// DFB_SURFACE_PIXELFORMAT(0, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0)  	16 bit ARGB (2 byte, alpha 1@15, red 5@10, green 5@5, blue 5@0)
	SPF_RGB16,		// DFB_SURFACE_PIXELFORMAT(1, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit RGB (2 byte, red 5@11, green 6@5, blue 5@0)
	SPF_RGB24,		// DFB_SURFACE_PIXELFORMAT(2, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)  	24 bit RGB (3 byte, red 8@16, green 8@8, blue 8@0)
	SPF_RGB32,		// DFB_SURFACE_PIXELFORMAT(3, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0)  	24 bit RGB (4 byte, nothing@24, red 8@16, green 8@8, blue 8@0)
	SPF_ARGB,			// DFB_SURFACE_PIXELFORMAT(4, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0)  	32 bit ARGB (4 byte, alpha 8@24, red 8@16, green 8@8, blue 8@0)
	SPF_A8,				// DFB_SURFACE_PIXELFORMAT(5, 0, 8, 1, 0, 1, 0, 0, 0, 0, 0)   	8 bit alpha (1 byte, alpha 8@0), e.g. anti-aliased glyphs
	SPF_YUY2,			// DFB_SURFACE_PIXELFORMAT(6, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit YUV (4 byte/ 2 pixel, macropixel contains CbYCrY [31:0])
	SPF_RGB332,		// DFB_SURFACE_PIXELFORMAT(7, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0)   	8 bit RGB (1 byte, red 3@5, green 3@2, blue 2@0)
	SPF_UYVY,			// DFB_SURFACE_PIXELFORMAT(8, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)  	16 bit YUV (4 byte/ 2 pixel, macropixel contains YCbYCr [31:0])
	SPF_I420,			// DFB_SURFACE_PIXELFORMAT(9, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)  	12 bit YUV (8 bit Y plane followed by 8 bit quarter size U/V planes)
	SPF_YV12,			// DFB_SURFACE_PIXELFORMAT(10, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by 8 bit quarter size V/U planes)
	SPF_LUT8,			// DFB_SURFACE_PIXELFORMAT(11, 8, 0, 1, 0, 1, 0, 0, 0, 1, 0)  	8 bit LUT (8 bit color and alpha lookup from palette)
	SPF_ALUT44,		// DFB_SURFACE_PIXELFORMAT(12, 4, 4, 1, 0, 1, 0, 0, 0, 1, 0)  	8 bit ALUT (1 byte, alpha 4@4, color lookup 4@0)
	SPF_AiRGB,		// DFB_SURFACE_PIXELFORMAT(13, 24, 8, 1, 0, 4, 0, 0, 0, 0, 1) 	32 bit ARGB (4 byte, inv. alpha 8@24, red 8@16, green 8@8, blue 8@0)
	SPF_A1,				// DFB_SURFACE_PIXELFORMAT(14, 0, 1, 1, 1, 0, 7, 0, 0, 0, 0)  	1 bit alpha (1 byte/ 8 pixel, most significant bit used first)
	SPF_NV12,			// DFB_SURFACE_PIXELFORMAT(15, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cb|Cr [7:0|7:0] plane)
	SPF_NV16,			// DFB_SURFACE_PIXELFORMAT(16, 24, 0, 0, 0, 1, 0, 0, 1, 0, 0) 	16 bit YUV (8 bit Y plane followed by one 16 bit half width Cb|Cr [7:0|7:0] plane)
	SPF_ARGB2554,	// DFB_SURFACE_PIXELFORMAT(17, 14, 2, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit ARGB (2 byte, alpha 2@14, red 5@9, green 5@4, blue 4@0)
	SPF_ARGB4444,	// DFB_SURFACE_PIXELFORMAT(18, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit ARGB (2 byte, alpha 4@12, red 4@8, green 4@4, blue 4@0)
	SPF_RGBA4444,	// DFB_SURFACE_PIXELFORMAT(19, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGBA (2 byte, red 4@12, green 4@8, blue 4@4, alpha 4@0)
	SPF_NV21,			// DFB_SURFACE_PIXELFORMAT(20, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0) 	12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cr|Cb [7:0|7:0] plane)
	SPF_AYUV,			// DFB_SURFACE_PIXELFORMAT(21, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0) 	32 bit AYUV (4 byte, alpha 8@24, Y 8@16, Cb 8@8, Cr 8@0)
	SPF_A4,				// DFB_SURFACE_PIXELFORMAT(22, 0, 4, 1, 4, 0, 1, 0, 0, 0, 0)  	4 bit alpha (1 byte/ 2 pixel, more significant nibble used first)
	SPF_ARGB1666,	// DFB_SURFACE_PIXELFORMAT(23, 18, 1, 1, 0, 3, 0, 0, 0, 0, 0) 	1 bit alpha (3 byte/ alpha 1@18, red 6@12, green 6@6, blue 6@0)
	SPF_ARGB6666,	// DFB_SURFACE_PIXELFORMAT(24, 18, 6, 1, 0, 3, 0, 0, 0, 0, 0) 	6 bit alpha (3 byte/ alpha 6@18, red 6@12, green 6@6, blue 6@0)
	SPF_RGB18,		// DFB_SURFACE_PIXELFORMAT(25, 18, 0, 0, 0, 3, 0, 0, 0, 0, 0) 	6 bit RGB (3 byte/ red 6@12, green 6@6, blue 6@0)
	SPF_LUT2,			// DFB_SURFACE_PIXELFORMAT(26, 2, 0, 1, 2, 0, 3, 0, 0, 1, 0)  	2 bit LUT (1 byte/ 4 pixel, 2 bit color and alpha lookup from palette)
	SPF_RGB444,		// DFB_SURFACE_PIXELFORMAT(27, 12, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGB (2 byte, nothing @12, red 4@8, green 4@4, blue 4@0)
	SPF_RGB555,		// DFB_SURFACE_PIXELFORMAT(28, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGB (2 byte, nothing @15, red 5@10, green 5@5, blue 5@0)
	SPF_BGR555,		// DFB_SURFACE_PIXELFORMAT(29, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0) 	16 bit BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0)
	SPF_RGBA5551,	// DFB_SURFACE_PIXELFORMAT(30, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0) 	16 bit RGBA (2 byte, red 5@11, green 5@6, blue 5@1, alpha 1@0)
	SPF_AVYU,			// DFB_SURFACE_PIXELFORMAT(31, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0) 	32 bit AVYU 4:4:4 (4 byte, alpha 8@24, Cr 8@16, Y 8@8, Cb 8@0)
	SPF_VYU,			// DFB_SURFACE_PIXELFORMAT(32, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)
};

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class OffScreenImage : public virtual jcommon::Object{

	private:
		uint8_t *_buffer;

	protected:
		Graphics *graphics;
		int _width,
			_height,
			_scale_width,
			_scale_height;
		jsurface_pixelformat_t _pixelformat;

	public:
		/**
		 * \brief
		 *
		 */
		OffScreenImage(int width, int height, jsurface_pixelformat_t pixelformat = SPF_ARGB, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~OffScreenImage();

		/**
		 * \brief
		 *
		 */
		Graphics * GetGraphics();

		/**
		 * \brief
		 *
		 */
		OffScreenImage * Create();

		/**
		 * \brief
		 *
		 */
		jsurface_pixelformat_t GetPixelFormat();

		/**
		 * \brief
		 *
		 */
		int GetWidth();
		
		/**
		 * \brief
		 *
		 */
		int GetHeight();

		/**
		 * \brief
		 *
		 */
		int GetScaleWidth();
		
		/**
		 * \brief
		 *
		 */
		int GetScaleHeight();

		/**
		 * \brief
		 *
		 */
		void Release();
		
		/**
		 * \brief
		 *
		 */
		void Restore();

};

}

#endif 
