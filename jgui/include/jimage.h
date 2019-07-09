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
    struct jsize_t<int> _size;
    /** \brief */
    jinterpolation_method_t _interpolation_method;
    /** \brief */
    jpixelformat_t _pixelformat;

  protected:
    /**
     * \brief
     *
     */
    Image(jpixelformat_t pixelformat, jsize_t<int> size);

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
    virtual jsize_t<int> GetSize();
    
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
    virtual Image * Shear(jsize_t<float> size);

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
    virtual void GetRGBArray(uint32_t *rgb, jrect_t<int> rect);
    
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
