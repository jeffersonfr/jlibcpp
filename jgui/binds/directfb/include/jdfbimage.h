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
#ifndef J_DFBIMAGE_H
#define J_DFBIMAGE_H

#include "jdfbimage.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DFBImage : public jgui::Image{

	private:
		uint8_t *_buffer;

	public:
		/**
		 * \brief
		 *
		 */
		DFBImage(int width, int height, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~DFBImage();

		/**
		 * \brief
		 *
		 */
		static bool GetImageSize(std::string img, int *width, int *height);
		
		/**
		 * \brief
		 *
		 */
		static Image * CreateImageStream(jio::InputStream *stream);

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
