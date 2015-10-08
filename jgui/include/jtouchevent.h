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

#include "jeventobject.h"
#include "jgraphics.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

namespace jgui {

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
class TouchEvent : public jcommon::EventObject{

	private:
		jpoint_t _location;
		jpoint_t _distance;
		double _pressure;
		double _delta;
		double _radians;
		int _finger_index;
		int _fingers;
		jtouchevent_type_t _type;

	public:
		/**
		 * \brief Touch event;
		 *
		 */
		TouchEvent(void *source, jtouchevent_type_t type, jpoint_t location, jpoint_t distance, double pressure, int finger_index);
		
		/**
		 * \brief Gesture event;
		 *
		 */
		TouchEvent(void *source, jtouchevent_type_t type, jpoint_t distance, double radians, double delta, int fingers);
		
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
		virtual jpoint_t GetLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetDistance();

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

