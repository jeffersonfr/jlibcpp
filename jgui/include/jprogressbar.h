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
#ifndef  J_PROGRESSBAR_H
#define J_PROGRESSBAR_H

#include "jgui/jslidercomponent.h"

namespace jgui {

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ProgressBar : public jgui::Component {

  private:
    /** \brief */
    jscroll_orientation_t _type;
    /** \brief */
    int _value;
    /** \brief */
    int _index;
    /** \brief */
    int _delta;
    /** \brief */
    int _fixe_delta;
    /** \brief */
    int _stone_size;

  public:
    /**
     * \brief
     *
     */
    ProgressBar(jscroll_orientation_t type = JSO_HORIZONTAL);
    
    /**
     * \brief
     *
     */
    virtual ~ProgressBar();

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
    virtual double GetValue();
    
    /**
     * \brief
     *
     */
    virtual void SetValue(double i);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

};

}

#endif

