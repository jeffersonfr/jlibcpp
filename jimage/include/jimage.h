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

#include "jmutex.h"

#include <string>

#include <stdint.h>

#define FLIP16(x) ((x >> 8) | (x << 8))
#define FLIP32(x) (((x & 0xff000000) >> 24) | ((x & 0x000000ff) << 24) | ((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8))

namespace jimage {

/**
 * \brief Type of image array. For get and set pixel functions
 * this enums masks the pixels.
 *
 */
enum jimage_type_t {
	BW_TYPE			= 0x00000001,
	GRAY_TYPE		= 0x00000002,
	RGB_TYPE		= 0x00000004,
	BGR_TYPE		= 0x00000008,
	ARGB_TYPE		= 0x00000010,
	RGBA_TYPE		= 0x00000020,
	UNKNOWN_TYPE	= 0x00000040
};

typedef uint32_t pixel_t;

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Image{

	protected:
		/** \brief */
		char *_header;
		/** \brief */
		int _header_length;
		/** \brief */
		uint8_t *_pixels;
		/** \brief */
		int _pixels_length;
		/** \brief */
		int _width;
		/** \brief */
		int _height;
		/** \brief */
		jimage_type_t _type;
		/** \brief */
		jthread::Mutex _mutex;
		
		/**
		 * \brief
		 * 
		 */
		Image();
		
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
		void SetPixel(int x, int y, pixel_t pixel);

		/**
		 * \brief
		 * 
		 */
		pixel_t GetPixel(int x, int y);

		/**
		 * \brief
		 * 
		 */
		void SetPixels(void **pixel, int *size_buffer);

		/**
		 * \brief Aloca um novo ponteiro contendo os pixels. Este novo ponteiro
		 * devera ser desalocado posteriormente.
		 * 
		 */
		void GetPixels(void **pixel, int *size_buffer);

		/**
		 * \brief
		 * 
		 */
		void Lock(uint8_t **pixel, int *size_buffer);

		/**
		 * \brief
		 * 
		 */
		void Unlock();

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
		jimage_type_t GetType();

		/**
		 * \brief Retorna os bytes de uma imagem, incluindo os headers.
		 *
		 */
		virtual void GetRawImage(void *img, int *size_img);

		/**
		 * \brief
		 *
		 */
		virtual std::string what();
    
};

}

#endif
