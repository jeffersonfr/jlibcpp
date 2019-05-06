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
#ifndef J_MOUSEEVENT_H
#define J_MOUSEEVENT_H

#include "jevent/jeventobject.h"
#include "jgui/jgraphics.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jmouseevent_type_t {
  JMT_UNKNOWN,
  JMT_PRESSED,
  JMT_RELEASED,
  JMT_MOVED,
  JMT_ROTATED
};

/**
 * \brief
 *
 */
enum jmouseevent_button_t {
  JMB_NONE    = 0x00,
  JMB_BUTTON1 = 0x01,
  JMB_BUTTON2 = 0x02,
  JMB_BUTTON3 = 0x04,
  JMB_WHEEL    = 0x08
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class MouseEvent : public jevent::EventObject {

  private:
    /** \brief */
    jgui::jpoint_t _location;
    /** \brief */
    jmouseevent_button_t _button;
    /** \brief */
    jmouseevent_button_t _buttons;
    /** \brief */
    jmouseevent_type_t _type;
    /** \brief */
    int _clicks;

  public:
    /**
     * \brief
     *
     */
    MouseEvent(void *source, jmouseevent_type_t type, jmouseevent_button_t button, jmouseevent_button_t buttons, jgui::jpoint_t location, int clicks);
    
    /**
     * \brief
     *
     */
    virtual ~MouseEvent();

    /**
     * \brief
     *
     */
    virtual jmouseevent_type_t GetType();

    /**
     * \brief
     *
     */
    virtual int GetClicks();
    
    /**
     * \brief
     *
     */
    virtual jmouseevent_button_t GetButton();

    /**
     * \brief
     *
     */
    virtual jmouseevent_button_t GetButtons();

    /**
     * \brief
     *
     */
    virtual jgui::jpoint_t GetLocation();
    
};

}

#endif

