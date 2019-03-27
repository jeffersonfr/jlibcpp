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

#include "jgui/jgraphics.h"
#include "jio/jfile.h"
#include "jio/jinputstream.h"

namespace jgui{

enum jinterpolation_method_t {
  JIM_NEAREST,
  JIM_BILINEAR,
  JIM_BICUBIC
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
class Image : public virtual jcommon::Object {

	protected:
		/** \brief */
		struct jsize_t _size;
		/** \brief */
    jinterpolation_method_t _interpolation_method;
		/** \brief */
		jpixelformat_t _pixelformat;

	protected:
		/**
		 * \brief
		 *
		 */
		Image(jpixelformat_t pixelformat, int width, int height);

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
		virtual jsize_t GetSize();
		
    /**
     * \brief
     *
     */
    virtual void SetInterpolationMethod(jinterpolation_method_t method);
    
    /**
     * \brief
     *
     */
    virtual jinterpolation_method_t GetInterpolationMethod();
    
		/**
		 * \brief
		 *
		 */
		virtual Image * Flip(jflip_flags_t mode);
		
		/**
		 * \brief
		 *
		 */
    virtual Image * Shear(float dx, float dy);

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
		virtual void GetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp);
		
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
		virtual jcommon::Object * Clone();

};

}

#endif 
