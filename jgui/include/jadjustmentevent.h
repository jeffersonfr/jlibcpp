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
#ifndef J_ADJUSTMENTEVENT_H
#define J_ADJUSTMENTEVENT_H

#include "jeventobject.h"

namespace jgui {

/**
 * \brief
 *
 */
enum jadjustmentevent_type_t {
	JAET_BLOCK_INCREMENT,
	JAET_BLOCK_DECREMENT,
	JAET_UNIT_INCREMENT,
	JAET_UNIT_DECREMENT
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class AdjustmentEvent : public jcommon::EventObject{

	private:
		/** \brief */
		jadjustmentevent_type_t _type;
		/** \brief */
		double _value;

	public:
		/**
		 * \brief
		 *
		 */
		AdjustmentEvent(void *source, jadjustmentevent_type_t type, double value);

		/**
		 * \brief
		 *
		 */
		virtual ~AdjustmentEvent();

		/**
		 * \brief
		 *
		 */
		virtual double GetValue();

		/**
		 * \brief
		 *
		 */
		virtual jadjustmentevent_type_t GetType();

};

}

#endif

