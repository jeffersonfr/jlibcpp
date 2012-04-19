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
#ifndef J_INDEXEDIMAGE_H
#define J_INDEXEDIMAGE_H

#include "jimage.h"

namespace jgui{

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class IndexedImage : public jgui::Image{

	private:
		uint32_t *_palette;
		int _palette_size;
		uint8_t *_data;

	public:
		/**
		 * \brief
		 *
		 */
		IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual ~IndexedImage();

		/**
		 * \brief Packs the source argb image.
		 *
		 */
		static IndexedImage * Pack(Image *image);

		/**
		 * \brief Packs the source argb image.
		 *
		 */
		static IndexedImage * Pack(uint32_t *rgb, int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual Image * Scaled(int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual Image * SubImage(int xp, int yp, int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual void GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetPalette(uint32_t **palette, int *size);

		/**
		 * \brief
		 *
		 */
		virtual void SetPalette(uint32_t *palette, int size);

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
