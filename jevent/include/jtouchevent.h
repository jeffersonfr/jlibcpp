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
#ifndef J_TOUCHEVENT_H
#define J_TOUCHEVENT_H

#include "jgui/jgraphics.h"
#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jtouchevent_type_t {
  JTT_UNKNOWN,
  JTT_FINGER_MOTION,
  JTT_FINGER_UP,
  JTT_FINGER_DOWN,
  JTT_GESTURE
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TouchEvent : public jevent::EventObject {

  private:
    /** \brief */
    jgui::jpoint_t _location;
    /** \brief */
    jgui::jpoint_t _distance;
    /** \brief */
    double _pressure;
    /** \brief */
    double _delta;
    /** \brief */
    double _radians;
    /** \brief */
    int _finger_index;
    /** \brief */
    int _fingers;
    /** \brief */
    jtouchevent_type_t _type;

  public:
    /**
     * \brief Touch event;
     *
     */
    TouchEvent(void *source, jtouchevent_type_t type, jgui::jpoint_t location, jgui::jpoint_t distance, double pressure, int finger_index);
    
    /**
     * \brief Gesture event;
     *
     */
    TouchEvent(void *source, jtouchevent_type_t type, jgui::jpoint_t distance, double radians, double delta, int fingers);
    
    /**
     * \brief
     *
     */
    virtual ~TouchEvent();

    /**
     * \brief
     *
     */
    virtual jtouchevent_type_t GetType();

    /**
     * \brief
     *
     */
    virtual int GetFingerIndex();
    
    /**
     * \brief
     *
     */
    virtual jgui::jpoint_t GetLocation();
    
    /**
     * \brief
     *
     */
    virtual jgui::jpoint_t GetDistance();

    /**
     * \brief
     *
     */
    virtual double GetPressure();

    /**
     * \brief
     *
     */
    virtual double GetAngle();

    /**
     * \brief
     *
     */
    virtual double GetDelta();

    /**
     * \brief
     *
     */
    virtual double GetFingers();

};

}

#endif

