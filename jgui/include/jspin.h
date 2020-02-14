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
#ifndef J_SPIN_H
#define J_SPIN_H

#include "jgui/jitemcomponent.h"
#include "jgui/jslidercomponent.h"
#include "jevent/jselectlistener.h"

#include <vector>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Spin : public jgui::Component, public jgui::ItemComponent {

  private:
    /** \brief */
    jscroll_orientation_t _type;

  private:
    void NextItem();
    void PreviousItem();

  public:
    /**
     * \brief
     *
     */
    Spin(jgui::jrect_t<int> bounds = {0, 0, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
    
    /**
     * \brief
     *
     */
    virtual ~Spin();

    /**
     * \brief
     *
     */
    virtual jscroll_orientation_t GetScrollOrientation();
    
    /**
     * \brief
     *
     */
    virtual void SetScrollOrientation(jscroll_orientation_t type);

    /**
     * \brief
     *
     */
    virtual void AddEmptyItem();
    
    /**
     * \brief
     *
     */
    virtual void AddTextItem(std::string text);
    
    /**
     * \brief
     *
     */
    virtual void AddImageItem(std::string text, jgui::Image *image);
    
    /**
     * \brief
     *
     */
    virtual void AddCheckedItem(std::string text, bool checked);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
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

};

}

#endif

