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
#ifndef J_BUFFEREDIMAGE_H
#define J_BUFFEREDIMAGE_H

#include "jgui/jimage.h"

#include <mutex>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class BufferedImage : public virtual jgui::Image {

	protected:
		/** \brief */
		Graphics *_graphics;
		/** \brief */
    std::mutex _mutex;

	public:
		/**
		 * \brief
		 *
		 */
		BufferedImage(jpixelformat_t pixelformat, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		BufferedImage(cairo_t *cairo_context);

		/**
		 * \brief
		 *
		 */
		BufferedImage(std::string file);

		/**
		 * \brief
		 *
		 */
		BufferedImage(jio::InputStream *stream);

		/**
		 * \brief
		 *
		 */
		virtual ~BufferedImage();

		/**
		 * \brief
		 *
		 */
		virtual Graphics * GetGraphics();

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
		virtual uint8_t * LockData();
	
		/**
		 * \brief
		 *
		 */
		virtual void UnlockData();
	
		/**
		 * \brief
		 *
		 */
		virtual jcommon::Object * Clone();

};

}

#endif 
