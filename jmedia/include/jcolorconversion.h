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
#ifndef J_COLORCONVERSION_H
#define J_COLORCONVERSION_H

#include "jobject.h"

namespace jmedia {

class ColorConversion : public virtual jcommon::Object {

	private:
		/**
		 * \brief
		 *
		 */
		ColorConversion();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~ColorConversion();

		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromGray(uint8_t **gray_array, uint32_t **rgb32_array, int width, int height);

		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromPalette(uint8_t **color_array, uint32_t **palette_array, uint32_t **rgb32_array, int width, int height);

		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromRGB16(uint8_t **rgb24_array, uint32_t **rgb32_array, int width, int height);

		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromRGB24(uint8_t **rgb24_array, uint32_t **rgb32_array, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromYV12(uint8_t **y_array, uint8_t **u_array, uint8_t **v_array, uint32_t **rgb32_array, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		static void GetRGB32FromYUYV(uint8_t **yuv_array, uint32_t **rgb32_array, int width, int height);

};

}

#endif
