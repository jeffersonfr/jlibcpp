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
#ifndef J_NATIVEIMAGE_H
#define J_NATIVEIMAGE_H

#include "jimage.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class NativeImage : public jgui::Image{

	private:
		/** \brief */
		uint8_t *_buffer;

	public:
		/**
		 * \brief
		 *
		 */
		NativeImage(void *surface, jpixelformat_t pixelformat, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		NativeImage(std::string file);
		
		/**
		 * \brief
		 *
		 */
		virtual ~NativeImage();

		/**
		 * \brief
		 *
		 */
		static jsize_t GetImageSize(std::string img);
		
		/**
		 * \brief
		 *
		 */
		static Image * CreateImageStream(jio::InputStream *stream);

		/**
		 * \brief
		 *
		 */
		static Image * Flip(Image *img, jflip_flags_t t);

		/**
		 * \brief
		 *
		 */
		static Image * Rotate(Image *img, double radians, bool resize);

		/**
		 * \brief
		 *
		 */
		static Image * Scale(Image *img, int width, int height);

		/**
		 * \brief
		 *
		 */
		static Image * Crop(Image *img, int x, int y, int width, int height);

		/**
		 * \brief
		 *
		 */
		static Image * Blend(Image *img, double alpha);

		/**
		 * \brief
		 *
		 */
		static Image * Colorize(Image *img, Color color);

		/**
		 * \brief
		 *
		 */
		virtual void SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride);

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

};

}

#endif 
