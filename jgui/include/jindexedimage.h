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

#include "jgui/jimage.h"

namespace jgui{

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class IndexedImage : public jgui::Image {

  private:
    /** \brief */
    uint32_t *_palette;
    /** \brief */
    int _palette_size;
    /** \brief */
    uint8_t *_data;

  public:
    /**
     * \brief
     *
     */
    IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, jsize_t<int> size);
    
    /**
     * \brief
     *
     */
    IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, jsize_t<int> size);
    
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
    static IndexedImage * Pack(uint32_t *rgb, jsize_t<int> size);

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
    virtual Image * Scale(jsize_t<int> size);

    /**
     * \brief
     *
     */
    virtual Image * Crop(jrect_t<int> rect);

    /**
     * \brief
     *
     */
    virtual Image * Blend(double alpha);

    /**
     * \brief
     *
     */
    virtual Image * Colorize(jcolor_t<float> color);

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
    virtual void GetRGBArray(uint32_t *rgb, jrect_t<int> rect);
    
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
    virtual jcommon::Object * Clone();

};

}

#endif 
