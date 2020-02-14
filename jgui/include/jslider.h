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
#ifndef  J_SLIDER_H
#define J_SLIDER_H

#include "jgui/jslidercomponent.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Slider : public jgui::SliderComponent {

  private:
    /** \brief */
    int _stone_size;
    /** \brief */
    bool _pressed;
    /** \brief */
    bool _inverted;

  public:
    /**
     * \brief
     *
     */
    Slider(jgui::jrect_t<int> bounds = {0, 0, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
    
    /**
     * \brief
     *
     */
    virtual ~Slider();

    /**
     * \brief
     *
     */
    virtual int GetStoneSize();
    
    /**
     * \brief
     *
     */
    virtual void SetStoneSize(int size);
    
    /**
     * \brief
     *
     */
    virtual void SetInverted(bool b);
    
    /**
     * \brief
     *
     */
    virtual bool KeyPressed(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

};

}

#endif

